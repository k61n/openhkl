import glob

headers = glob.glob('**/*.h', recursive=True)

for header in headers:
    print('#include "' + header + '"')

print()

for header in headers:
    print('%include "' + header + '"')
