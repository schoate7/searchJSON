NOTE: Requires nlohmann/json library to compile.

Usage:
Linux/Mac: ./searchJSON <filename.json> <search1> [search2] [search3]
Windows (Powershell) .\searchJSON.exe <filename.json> <search1> [search2] [search3]

Search for an entire string, pass string as argument, ex: ./searchJSON myfile.json mystring
Search for a key value pair, search arg key=value, ex: ./searchJSON myfile.json userid=abc123
