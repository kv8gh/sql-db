# Check if the .vscode folder exists, if not, create it
if (!(Test-Path -Path ".vscode")) {
    New-Item -ItemType Directory -Path ".vscode"
}

# Create the configuration file
@"
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/include",
                "${workspaceFolder}/sql",
                "${workspaceFolder}/storage",
                "${workspaceFolder}/executor"
            ],
            "compilerPath": "g++",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-x64"
        }
    ],
    "version": 4
}
"@ | Out-File -Encoding UTF8 .vscode/c_cpp_properties.json

Write-Output "✅ Configuration file created successfully!"
