import os

def replace_string_in_files(root_folder, target_string, replacement_string, filename_contains):
    for subdir, _, files in os.walk(root_folder):
        for file in files:
            if filename_contains in file:
                file_path = os.path.join(subdir, file)
                with open(file_path, 'r') as f:
                    content = f.read()
                content = content.replace(target_string, replacement_string)
                with open(file_path, 'w') as f:
                    f.write(content)

# Example usage
root_folder = '.' # Replace with the path to your folder
replace_string_in_files(root_folder, 'Ref', 'Mid', 'PIK')

print("String replacement completed.")
