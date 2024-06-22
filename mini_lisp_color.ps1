try {
    # Define path to mini_lisp.exe
    $exePath = "D:\Users\Downloads\mini_lisp\build\mini_lisp.exe"

    # Create ProcessStartInfo object
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = $exePath
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardInput = $true
    $psi.UseShellExecute = $false
    $psi.CreateNoWindow = $true

    # Start the process
    $process = [System.Diagnostics.Process]::Start($psi)

    Write-Host "mini_lisp.exe process started."

    # Read and process output from mini_lisp interpreter
    while (!$process.StandardOutput.EndOfStream) {
        $output = $process.StandardOutput.ReadLine().Trim()
        Write-Host "Received output: $output"

        # Process output as needed
        # ...

        # Example: Check for specific prompt or output
        if ($output -match ">>>") {
            $input = Read-Host "Enter your command:"
            Write-Host "Sending input: $input"
            $process.StandardInput.WriteLine($input)
        }
    }

    # Optionally wait for the process to exit
    $process.WaitForExit()

} catch {
    Write-Host "Error occurred: $_"
}
