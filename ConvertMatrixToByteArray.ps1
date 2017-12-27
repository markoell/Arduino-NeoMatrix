﻿function Format-Binary(){

    Process{
        [Convert]::ToString($PSItem, 2).PadLeft(8,'0')
    }
}

$arr2118 = (
  ( 0x7, 0xE0, 0x82, 0x3E ),
  ( 0xF, 0xF1, 0x86, 0x7F ),
  ( 0xC, 0x33, 0x8E, 0x63 ),
  ( 0x0, 0x37, 0x9E, 0x63 ),
  ( 0x0, 0x31, 0x86, 0x63 ),
  ( 0x0, 0x31, 0x86, 0x63 ),
  ( 0x0, 0x71, 0x86, 0x7F ),
  ( 0x0, 0xE1, 0x86, 0x3E ),
  ( 0x3, 0x81, 0x86, 0x7F ),
  ( 0x7, 0x01, 0x86, 0x63 ),
  ( 0xE, 0x01, 0x86, 0x63 ),
  ( 0xC, 0x01, 0x86, 0x63 ),
  ( 0xC, 0x01, 0x86, 0x63 ),
  ( 0xC, 0x01, 0x86, 0x63 ),
  ( 0xF, 0xF3, 0xCF, 0x7F ),
  ( 0xF, 0xF3, 0xCF, 0x3E )
);

$arr1884 = (
  ( 0x1, 0x1F, 0x1E, 0x36 ),
  ( 0x3, 0x3F, 0xBF, 0xB6 ),
  ( 0x7, 0x31, 0xB1, 0xB6 ),
  ( 0xF, 0x31, 0xB1, 0xB6 ),
  ( 0x3, 0x31, 0xB1, 0xB6 ),
  ( 0x3, 0x31, 0xB1, 0xB6 ),
  ( 0x3, 0x3F, 0xBF, 0xBF ),
  ( 0x3, 0x1F, 0x1F, 0x3F ),
  ( 0x3, 0x3F, 0xBF, 0x86 ),
  ( 0x3, 0x31, 0xB1, 0x86 ),
  ( 0x3, 0x31, 0xB1, 0x86 ),
  ( 0x3, 0x31, 0xB1, 0x86 ),
  ( 0x3, 0x31, 0xB1, 0x86 ),
  ( 0x3, 0x31, 0xB1, 0x86 ),
  ( 0x7, 0xBF, 0xBF, 0x86 ),
  ( 0x7, 0x9F, 0x1F, 0x60 )
);

$arr1984 = (
  ( 0x1, 0x1F, 0x1E, 0x36 ),
  ( 0x3, 0x3F, 0xBF, 0xB6 ),
  ( 0x7, 0x31, 0xB1, 0xB6 ),
  ( 0xF, 0x31, 0xB1, 0xB6 ),
  ( 0x3, 0x31, 0xB1, 0xB6 ),
  ( 0x3, 0x31, 0xB1, 0xB6 ),
  ( 0x3, 0x3F, 0xBF, 0xBF ),
  ( 0x3, 0x3F, 0x9F, 0x3F ),
  ( 0x3, 0x1F, 0xBF, 0x86 ),
  ( 0x3, 0x01, 0xB1, 0x86 ),
  ( 0x3, 0x01, 0xB1, 0x86 ),
  ( 0x3, 0x01, 0xB1, 0x86 ),
  ( 0x3, 0x01, 0xB1, 0x86 ),
  ( 0x3, 0x31, 0xB1, 0x86 ),
  ( 0x7, 0xBF, 0xBF, 0x86 ),
  ( 0x7, 0x9F, 0x1F, 0x06 )
);

$all = ($arr1884, $arr1984, $arr2118)


 $all | % { $_ | % { $_ | % { Write-Host -NoNewline $($_ | Format-Binary) }; Write-Host }; Write-Host }