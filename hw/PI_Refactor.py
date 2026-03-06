import os
import re

def replace_headers_in_files(root_folder):
   pattern = re.compile(r"^.{7}_PIK_\d{4}-\d{2}-\d{2}\.csv$")

    for subdir, _, files in os.walk(root_folder):
        for file in files:
            if pattern.match(file):
                file_path = os.path.join(subdir, file)

                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()

                # Nur die gewünschten Header ersetzen
                content = content.replace("Ref-X(mm)", "Mid-X(mm)")
                content = content.replace("Ref-Y(mm)", "Mid-Y(mm)")

                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)

# Beispielaufruf
replace_headers_in_files('.',)

print("Header-Anpassung abgeschlossen.")
