Get-ChildItem -Path ..\src -Recurse -Include *.cpp, *.h | Where-Object { $_.FullName -notlike "*\protobuf\*" } | ForEach-Object { clang-format -i $_.FullName }