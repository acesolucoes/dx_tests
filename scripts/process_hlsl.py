import os, sys

def read_and_escape_hlsl(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        
    escaped_lines = []
    for line in lines:
        # Escape backslashes and double quotes
        escaped_line = line.replace('\\', '\\\\').replace('"', '\\"')
        escaped_lines.append(escaped_line)
    
    return ''.join(escaped_lines)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: ", sys.argv[0], " file.hlsl")
        sys.exit(1)
    
    hlsl_path = sys.argv[1]
    escaped_hlsl_content = read_and_escape_hlsl(hlsl_path)

    print(f'const char* shaderCode = "{escaped_hlsl_content}";')
