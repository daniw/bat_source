.PHONY: all build vim count clean

all: build count

build: 
	mkdir -p obj
	mkdir -p obj/src
	if [ -e obj/bat_source.aux ];\
	then\
		cp obj/bat_source.aux obj/bat_source_old.aux;\
	fi
	pdflatex -jobname bat_source -output-directory obj bat_source.tex
	cd obj && bibtex bat_source.aux
	latex_count=5 ; \
	while ! cmp -s obj/bat_source.aux obj/bat_source_old.aux && [ $$latex_count -gt 0 ] ;\
	do \
		echo "Rerunning latex....." ;\
		cp obj/bat_source.aux obj/bat_source_old.aux;\
		pdflatex -jobname bat_source -output-directory obj bat_source.tex ;\
		latex_count=`expr $$latex_count - 1` ;\
	done

vim: 
	mkdir -p obj
	mkdir -p obj/src
	if [ -e obj/bat_source.aux ];\
	then\
		cp obj/bat_source.aux obj/bat_source_old.aux;\
	fi
	pdflatex -jobname bat_source -output-directory obj -halt-on-error bat_source.tex
	cd obj && bibtex bat_source.aux
	latex_count=5 ; \
	while ! cmp -s obj/bat_source.aux obj/bat_source_old.aux && [ $$latex_count -gt 0 ] ;\
	do \
		echo "Rerunning latex....." ;\
		cp obj/bat_source.aux obj/bat_source_old.aux;\
		pdflatex -jobname bat_source -output-directory obj -halt-on-error bat_source.tex ;\
		latex_count=`expr $$latex_count - 1` ;\
	done

count:
	@echo
	@echo Counting words in abstract:
	@wc src/abstract_de.tex src/abstract_en.tex | grep abstract

clean:
	rm -f obj/*.aux
	rm -f obj/src/*.aux
	rm -f obj/*.bbl
	rm -f obj/*.blg
	rm -f obj/*.lof
	rm -f obj/*.log
	rm -f obj/*.lot
	rm -f obj/*.out
	rm -f obj/*.tdo
	rm -f obj/*.toc

cleanpdf:
	rm -f obj/*.aux
	rm -f obj/src/*.aux
	rm -f obj/*.bbl
	rm -f obj/*.blg
	rm -f obj/*.lof
	rm -f obj/*.log
	rm -f obj/*.lot
	rm -f obj/*.out
	rm -f obj/*.tdo
	rm -f obj/*.toc
	rm -f obj/*.pdf
