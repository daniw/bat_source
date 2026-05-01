/*
 * w25n01gv.c
 *
 *  Created on: Apr 20, 2026
 *      Author: daniw
 */

#include "w25n01gv.h"

/*
 * Initialize flash memory W25N01GV
 * @param hw25n01gv W25N01GV handle to the device.
 * @param hqspi handle for QSPI peripheral
 * @param timeout timeout for QSPI operation
 */
HAL_StatusTypeDef w25n01gv_init(w25n01gv_handle *hw25n01gv, QSPI_HandleTypeDef *hqspi, uint32_t timeout) {
#ifdef W25N01GV_TEST_UNION_STRUCT
	/* test orientation of bitfields */
	test_bitfield_t test_bitfield;      /* testvariable */
	test_bitfield.byte[0] = 0x00;
	test_bitfield.bitfield.low = 0x01;  /* write testvalue to lower nibble */
	if (test_bitfield.byte[0] != 0x01) {   /* test if lower nibble has been written */
		while (1) {
			/* Hey Programmer,
			 * It seems that your compiler uses a
			 * different order for bitfields than mine.
			 * If you still want to use this library,
			 * change, if possible, the order of bitfields in
			 * your compiler or in this library!
			 */
		}
	}
	test_bitfield.word = 0x0000;
	test_bitfield.byte[0] = 0x55;  /* write testvalue to lower byte */
	if (test_bitfield.word != 0x0055) {   /* test if lower byte has been written */
		while (1) {
			/* Hey Programmer,
			 * It seems that your compiler uses a
			 * different order for bitfields than mine.
			 * If you still want to use this library,
			 * change, if possible, the order of bitfields in
			 * your compiler or in this library!
			 */
		}
	}
#endif // W25N01GV_TEST_UNION_STRUCT
	HAL_StatusTypeDef status;
	hw25n01gv->hqspi = hqspi;
	hw25n01gv->timeout = timeout;
	hw25n01gv->reg_prot.reg = W25N01GV_REG_PROT_INIT;
	hw25n01gv->reg_conf.reg = W25N01GV_REG_CONF_INIT;
	hw25n01gv->reg_status.reg = W25N01GV_REG_STATUS_INIT;
	hw25n01gv->reg_conf.fields.buf_mode = W25N01GV_BUF;
	status = w25n01gv_reset(hw25n01gv);
	if (status == HAL_OK) {
		status = w25n01gv_read_reg(hw25n01gv);
	}
	return status;
}

HAL_StatusTypeDef w25n01gv_reset(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_RESET;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_NONE;
	cmd.NbData              = 0;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = w25n01gv_wait_busy(hw25n01gv);
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	/*if (status == HAL_OK) {
		uint8_t pData;
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, &pData, hw25n01gv->timeout);
	}*/
	return status;
}

/*
 * Read JEDEC ID
 * @param hw25n01gv W25N01GV handle to the device.
 * @param id pointer to array to write JEDEC ID into
 */
HAL_StatusTypeDef w25n01gv_read_id(w25n01gv_handle *hw25n01gv, uint8_t *id) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_READ_ID;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 8;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 3;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Receive(hw25n01gv->hqspi, id, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Read all status registers
 * @param hw25n01gv W25N01GV handle to the device.
 */
HAL_StatusTypeDef w25n01gv_read_reg(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	status = w25n01gv_read_reg_prot(hw25n01gv);
	if (status == HAL_OK) {
		status = w25n01gv_read_reg_conf(hw25n01gv);
	}
	if (status == HAL_OK) {
		status = w25n01gv_read_reg_status(hw25n01gv);
	}
	return status;
}

/*
 * Read protection register
 * @param hw25n01gv W25N01GV handle to the device.
 * @param reg pointer to register variable
 */
HAL_StatusTypeDef w25n01gv_read_reg_prot(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_READ_STATUS_REG;
	cmd.Address             = W25N01GV_REG_PROT_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Receive(hw25n01gv->hqspi, &hw25n01gv->reg_prot.reg, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Read configuration register
 * @param hw25n01gv W25N01GV handle to the device.
 * @param reg pointer to register variable
 */
HAL_StatusTypeDef w25n01gv_read_reg_conf(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_READ_STATUS_REG;
	cmd.Address             = W25N01GV_REG_CONF_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Receive(hw25n01gv->hqspi, &hw25n01gv->reg_conf.reg, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Read status register
 * @param hw25n01gv W25N01GV handle to the device.
 * @param reg pointer to register variable
 */
HAL_StatusTypeDef w25n01gv_read_reg_status(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_READ_STATUS_REG;
	cmd.Address             = W25N01GV_REG_STATUS_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Receive(hw25n01gv->hqspi, &hw25n01gv->reg_status.reg, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Write all status registers
 * @param hw25n01gv W25N01GV handle to the device.
 */
HAL_StatusTypeDef w25n01gv_write_reg(w25n01gv_handle *hw25n01gv, w25n01gv_reg_t *regs) {
	HAL_StatusTypeDef status;
	status = w25n01gv_write_reg_prot(hw25n01gv, &regs->reg_prot);
	if (status == HAL_OK) {
		status = w25n01gv_write_reg_conf(hw25n01gv, &regs->reg_conf);
	}
	if (status == HAL_OK) {
		status = w25n01gv_write_reg_status(hw25n01gv, &regs->reg_status);
	}
	return status;
}

/*
 * Write protection register
 * @param hw25n01gv W25N01GV handle to the device.
 * @param reg pointer to register variable
 */
HAL_StatusTypeDef w25n01gv_write_reg_prot(w25n01gv_handle *hw25n01gv, w25n01gv_prot_reg_t *reg) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_WRITE_STATUS_REG;
	cmd.Address             = W25N01GV_REG_PROT_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, &reg->reg, hw25n01gv->timeout);
	}
	if (status == HAL_OK) {
		hw25n01gv->reg_prot.reg = reg->reg;
	}
	return status;
}

/*
 * Write configuration register
 * @param hw25n01gv W25N01GV handle to the device.
 * @param reg pointer to register variable
 */
HAL_StatusTypeDef w25n01gv_write_reg_conf(w25n01gv_handle *hw25n01gv, w25n01gv_conf_reg_t *reg) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_WRITE_STATUS_REG;
	cmd.Address             = W25N01GV_REG_CONF_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, &reg->reg, hw25n01gv->timeout);
	}
	if (status == HAL_OK) {
		hw25n01gv->reg_conf.reg = reg->reg;
	}
	return status;
}

/*
 * Write status register
 * @param hw25n01gv W25N01GV handle to the device.
 * @param reg pointer to register variable
 */
HAL_StatusTypeDef w25n01gv_write_reg_status(w25n01gv_handle *hw25n01gv, w25n01gv_status_reg_t *reg) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_WRITE_STATUS_REG;
	cmd.Address             = W25N01GV_REG_STATUS_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, &reg->reg, hw25n01gv->timeout);
	}
	if (status == HAL_OK) {
		hw25n01gv->reg_status.reg = reg->reg;
	}
	return status;
}

/*
 * Write protection register
 * @param hw25n01gv W25N01GV handle to the device.
 * @param reg pointer to register variable
 */
HAL_StatusTypeDef w25n01gv_set_prot(w25n01gv_handle *hw25n01gv, uint8_t block_prot, uint8_t upper_lower) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_READ_STATUS_REG;
	cmd.Address             = W25N01GV_REG_PROT_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Receive(hw25n01gv->hqspi, &hw25n01gv->reg_prot.reg, hw25n01gv->timeout);
		hw25n01gv->reg_prot.fields.block_prot = block_prot & 0xF;
		hw25n01gv->reg_prot.fields.top_bot_prot = upper_lower & 0x1;
	}
	cmd.Instruction         = W25N01GV_CMD_WRITE_STATUS_REG;
	cmd.Address             = W25N01GV_REG_PROT_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	if (status == HAL_OK) {
		status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	}
	if (status == HAL_OK) {
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, &hw25n01gv->reg_prot.reg, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Wait until the busy flag in the status register has cleared
 * @param hw25n01gv W25N01GV handle to the device.
 */
HAL_StatusTypeDef w25n01gv_wait_busy(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	w25n01gv_status_reg_t reg;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_READ_STATUS_REG;
	cmd.Address             = W25N01GV_REG_STATUS_ADDR;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 1;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	do {
		status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
		if (status == HAL_OK) {
			//status = HAL_QSPI_Receive(hw25n01gv->hqspi, &reg.reg, hw25n01gv->timeout);
			status = HAL_QSPI_Receive(hw25n01gv->hqspi, &hw25n01gv->reg_status.reg, hw25n01gv->timeout);
		}
	//} while ((status == HAL_OK) && (reg.fields.busy != 0));
	} while ((status == HAL_OK) && (hw25n01gv->reg_status.fields.busy != 0));
	return status;
}

/*
 * Enable buffer mode
 * @param hw25n01gv W25N01GV handle to the device.
 */
HAL_StatusTypeDef w25n01gv_buffer_mode_enable(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	status = w25n01gv_read_reg_conf(hw25n01gv);
	if (status == HAL_OK) {
		hw25n01gv->reg_conf.fields.buf_mode = 1;
		status = w25n01gv_write_reg_conf(hw25n01gv, &hw25n01gv->reg_conf);
	}
	return status;
}

/*
 * Disable buffer mode
 * @param hw25n01gv W25N01GV handle to the device.
 */
HAL_StatusTypeDef w25n01gv_buffer_mode_disable(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	status = w25n01gv_read_reg_conf(hw25n01gv);
	if (status == HAL_OK) {
		hw25n01gv->reg_conf.fields.buf_mode = 0;
		status = w25n01gv_write_reg_conf(hw25n01gv, &hw25n01gv->reg_conf);
	}
	return status;
}

/*
 * Enable write
 * @param hw25n01gv W25N01GV handle to the device.
 */
HAL_StatusTypeDef w25n01gv_write_enable(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_WRITE_ENABLE;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_NONE;
	cmd.NbData              = 0;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	return status;
}

/*
 * Disable write
 * @param hw25n01gv W25N01GV handle to the device.
 */
HAL_StatusTypeDef w25n01gv_write_disable(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_WRITE_DISABLE;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_NONE;
	cmd.NbData              = 0;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	return status;
}

/*
 * Bad block management
 * @param hw25n01gv W25N01GV handle to the device.
 * @param badblock  Pointer to bad block to be written to bad block look up table
 * Todo: Untested, must be tested before usage
 */
HAL_StatusTypeDef w25n01gv_badblock_management(w25n01gv_handle *hw25n01gv, w25n01gv_badblock_t badblock) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_BB_MANAGEMENT;
	cmd.Address             = badblock.word;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_32_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 0;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_NONE;
	cmd.NbData              = 0;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		uint8_t pData;
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, &pData, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Read bad block look up table
 * @param hw25n01gv W25N01GV handle to the device.
 */
HAL_StatusTypeDef w25n01gv_read_badblock(w25n01gv_handle *hw25n01gv) {
	HAL_StatusTypeDef status;
	uint8_t pData[4*W25N01GV_BADBLOCK_LUT_SIZE];
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_READ_BBM_LUT;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 8;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 4*W25N01GV_BADBLOCK_LUT_SIZE;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Receive(hw25n01gv->hqspi, pData, hw25n01gv->timeout);
	}
	if (status == HAL_OK) {
		for (uint8_t i = 0; i < W25N01GV_BADBLOCK_LUT_SIZE; i++) {
			for (uint8_t j = 0; j < 4; j++) {
				hw25n01gv->badblock_lut->bytes[3-j] = pData[i*4 + j];
			}
		}
	}
	return status;
}

/*
 * Read page address of the last ECC failure
 * @param hw25n01gv W25N01GV handle to the device.
 * @param page_addr pointer for page address of last ECC failure
 */
HAL_StatusTypeDef w25n01gv_last_ecc_failure(w25n01gv_handle *hw25n01gv, uint16_t *page_addr) {
	HAL_StatusTypeDef status;
	uint8_t pData[2];
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_LAST_ECC_FAIL_PAGE_ADDR;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 8;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 2;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Receive(hw25n01gv->hqspi, pData, hw25n01gv->timeout);
	}
	if (status == HAL_OK) {
		*page_addr = (((uint16_t) pData[0]) << 8) | pData[1];
	}
	return status;
}

/*
 * Block erase
 * @param hw25n01gv W25N01GV handle to the device.
 * @param page_addr Page address
 */
HAL_StatusTypeDef w25n01gv_block_erase(w25n01gv_handle *hw25n01gv, uint16_t page_addr) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_BLOCK_ERASE;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 8;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 2;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		uint8_t pData[2];
		pData[0] = (uint8_t) (page_addr>>8);
		pData[1] = (uint8_t) (page_addr & 0x00FF);
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, pData, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Load data into data buffer for preparation to program execution
 * @param hw25n01gv W25N01GV handle to the device
 * @param col_addr column address
 * @param data pointer to data
 * @param col_addr column address
 * @param len length of data to be loaded
 */
HAL_StatusTypeDef w25n01gv_prog_load(w25n01gv_handle *hw25n01gv, uint8_t *data, uint16_t col_addr, uint32_t len) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
#if (W25N01GV_WRITE_DATA_LINES == 1)
		cmd.Instruction         = W25N01GV_CMD_PROG_DATA_LOAD;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 0;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_1_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_WRITE_DATA_LINES == 4)
		cmd.Instruction         = W25N01GV_CMD_QUAD_PROG_DATA_LOAD;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 0;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_4_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#else
	#error This amount of data lines is not supported! Check the definition of W25N01GV_WRITE_DATA_LINES.
#endif // W25N01GV_WRITE_DATA_LINES
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, data, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Load data into data buffer for preparation to program execution
 * @param hw25n01gv W25N01GV handle to the device
 * @param col_addr column address
 * @param data pointer to data
 * @param col_addr column address
 * @param len length of data to be loaded
 */
HAL_StatusTypeDef w25n01gv_prog_load_random(w25n01gv_handle *hw25n01gv, uint8_t *data, uint16_t col_addr, uint32_t len) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
#if (W25N01GV_WRITE_DATA_LINES == 1)
		cmd.Instruction         = W25N01GV_CMD_RAND_PROG_DATA_LOAD;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 0;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_1_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_WRITE_DATA_LINES == 4)
		cmd.Instruction         = W25N01GV_CMD_RAND_QUAD_PROG_DATA_LOAD;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 0;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_4_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#else
	#error This amount data lines is not supported! Check the definition of W25N01GV_WRITE_DATA_LINES.
#endif // W25N01GV_WRITE_DATA_LINES
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, data, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Program execute
 * @param hw25n01gv W25N01GV handle to the device.
 * @param page_addr Page address
 */
HAL_StatusTypeDef w25n01gv_prog_exec(w25n01gv_handle *hw25n01gv, uint16_t page_addr) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_PROG_EXEC;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 8;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 2;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		uint8_t pData[2];
		pData[0] = (uint8_t) (page_addr>>8);
		pData[1] = (uint8_t) (page_addr & 0x00FF);
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, pData, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Page data read from memory page into data buffer
 * @param hw25n01gv W25N01GV handle to the device
 * @param addr page address
 */
HAL_StatusTypeDef w25n01gv_page_read(w25n01gv_handle *hw25n01gv, uint16_t page_addr) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	cmd.Instruction         = W25N01GV_CMD_PAGE_DATA_READ;
	cmd.Address             = 0x00;
	cmd.AlternateBytes      = 0x00;
	cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
	cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
	cmd.DummyCycles         = 8;
	cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
	cmd.AddressMode         = QSPI_ADDRESS_NONE;
	cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode            = QSPI_DATA_1_LINE;
	cmd.NbData              = 2;
	cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		uint8_t pData[2];
		pData[0] = (uint8_t) (page_addr>>8);
		pData[1] = (uint8_t) (page_addr & 0x00FF);
		status = HAL_QSPI_Transmit(hw25n01gv->hqspi, pData, hw25n01gv->timeout);
	}
	return status;
}

/*
 * Read data from data buffer
 * @param hw25n01gv W25N01GV handle to the device
 * @param data pointer to data
 * @param col_addr column address
 * @param len length of data to be read
 */
HAL_StatusTypeDef w25n01gv_read(w25n01gv_handle *hw25n01gv, uint8_t *data, uint16_t col_addr, uint32_t len) {
	HAL_StatusTypeDef status;
	QSPI_CommandTypeDef cmd = {0};
	if ((hw25n01gv->reg_conf.fields.buf_mode == 1) || (hw25n01gv->reg_conf.fields.otp_enter == 1)) {
#if (W25N01GV_FAST_READ == 0)
		cmd.Instruction         = W25N01GV_CMD_READ;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 8;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_1_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 1) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 8;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_1_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 1) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_4_BYTE_ADDR;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 24;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_1_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 2) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_DUAL_OUT;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 8;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_2_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 2) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_DUAL_OUT_4_BYTE_ADDR;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 24;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_2_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 4) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_QUAD_OUT;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 8;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_4_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 4) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_QUAD_OUT_4_BYTE_ADDR;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 24;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_4_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 2) && (W25N01GV_READ_DATA_LINES == 2) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_DUAL_IO;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 4;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_2_LINES;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_2_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 2) && (W25N01GV_READ_DATA_LINES == 2) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_DUAL_IO_4_BYTE_ADDR;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 12;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_2_LINES;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_2_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 4) && (W25N01GV_READ_DATA_LINES == 4) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_QUAD_IO;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 4;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_4_LINES;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_4_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 4) && (W25N01GV_READ_DATA_LINES == 4) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_QUAD_IO_4_BYTE_ADDR;
		cmd.Address             = col_addr;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 10;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_4_LINES;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_4_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#else
	#error This combination of address and data lines is not supported! Check the definition of W25N01GV_READ_ADDR_LINES, W25N01GV_READ_DATA_LINES, W25N01GV_READ_4_BYTE_ADDR.
#endif // W25N01GV_READ_ADDR_LINES, W25N01GV_READ_DATA_LINES, W25N01GV_READ_4_BYTE_ADDR
	}
	else { // ((hw25n01gv->reg_conf.fields.buf_mode == 0) && (hw25n01gv->reg_conf.fields.otp_enter == 0))
#if (W25N01GV_FAST_READ == 0)
		cmd.Instruction         = W25N01GV_CMD_READ;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 24;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_1_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 1) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_8_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_32_BITS;
		cmd.DummyCycles         = 0;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_1_LINE;
		cmd.DataMode            = QSPI_DATA_1_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 1) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_4_BYTE_ADDR;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_32_BITS;
		cmd.DummyCycles         = 8;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_1_LINE;
		cmd.DataMode            = QSPI_DATA_1_LINE;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 2) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_DUAL_OUT;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_32_BITS;
		cmd.DummyCycles         = 0;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_1_LINE;
		cmd.DataMode            = QSPI_DATA_2_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 2) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_DUAL_OUT_4_BYTE_ADDR;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_32_BITS;
		cmd.DummyCycles         = 8;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_1_LINE;
		cmd.DataMode            = QSPI_DATA_2_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 4) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_QUAD_OUT;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_32_BITS;
		cmd.DummyCycles         = 0;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_1_LINE;
		cmd.DataMode            = QSPI_DATA_4_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 1) && (W25N01GV_READ_DATA_LINES == 4) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_QUAD_OUT_4_BYTE_ADDR;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_32_BITS;
		cmd.DummyCycles         = 8;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_1_LINE;
		cmd.DataMode            = QSPI_DATA_4_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 2) && (W25N01GV_READ_DATA_LINES == 2) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_DUAL_IO;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 16;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_2_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 2) && (W25N01GV_READ_DATA_LINES == 2) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_DUAL_IO_4_BYTE_ADDR;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 20;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_2_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 4) && (W25N01GV_READ_DATA_LINES == 4) && (W25N01GV_READ_4_BYTE_ADDR == 0)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_QUAD_IO;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 12;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_4_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#elif (W25N01GV_READ_ADDR_LINES == 4) && (W25N01GV_READ_DATA_LINES == 4) && (W25N01GV_READ_4_BYTE_ADDR == 1)
		cmd.Instruction         = W25N01GV_CMD_FAST_READ_QUAD_IO_4_BYTE_ADDR;
		cmd.Address             = 0x00;
		cmd.AlternateBytes      = 0x00;
		cmd.AddressSize         = QSPI_ADDRESS_16_BITS;
		cmd.AlternateBytesSize  = QSPI_ALTERNATE_BYTES_8_BITS;
		cmd.DummyCycles         = 14;
		cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode         = QSPI_ADDRESS_NONE;
		cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
		cmd.DataMode            = QSPI_DATA_4_LINES;
		cmd.NbData              = len;
		cmd.DdrMode             = QSPI_DDR_MODE_DISABLE;
		cmd.DdrHoldHalfCycle    = QSPI_DDR_HHC_ANALOG_DELAY;
		cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;
#else
	#error This combination of address and data lines is not supported! Check the definition of W25N01GV_READ_ADDR_LINES, W25N01GV_READ_DATA_LINES, W25N01GV_READ_4_BYTE_ADDR.
#endif // W25N01GV_READ_ADDR_LINES, W25N01GV_READ_DATA_LINES, W25N01GV_READ_4_BYTE_ADDR
	}
	status = HAL_QSPI_Command(hw25n01gv->hqspi, &cmd, hw25n01gv->timeout);
	if (status == HAL_OK) {
		status = HAL_QSPI_Receive(hw25n01gv->hqspi, data, hw25n01gv->timeout);
	}
	return status;
}
