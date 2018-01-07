

Function Convert-Byte2HexString ([string[]] $value) {

    $($value | % {
        "0x" + $("{0:x2}" -f ([convert]::ToByte($_,2))).ToUpper()
    }) -join ", "

}

Get-Content -Path "C:\Users\mk\Documents\Arduino\LEDMatrix\AnzeigeWerte.txt" | % {

    if ($_ -match "^#" ) {
        "};"
        ""
        "const uint8_t $($_ -replace '#','')[][4] = {"
    }
    elseif( $_ -match "^$" ){
    }
    else {
        $groups = $($_ -replace ' ','0') -match "(\d{4})(\d{8})(\d{8})(\d{8})"
        
        "  { " + $(Convert-Byte2HexString(($Matches[1],$Matches[2], $Matches[3], $Matches[4]))) + " },"
        
        #
    }

}| clip.exe