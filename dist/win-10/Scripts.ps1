function Get-PublisherHash($publisherName)
{
    $publisherNameAsUnicode = [System.Text.Encoding]::Unicode.GetBytes($publisherName);
    $publisherSha256 = [System.Security.Cryptography.HashAlgorithm]::Create("SHA256").ComputeHash($publisherNameAsUnicode);
    $publisherSha256First8Bytes = $publisherSha256 | Select-Object -First 8;
    $publisherSha256AsBinary = $publisherSha256First8Bytes | ForEach-Object { [System.Convert]::ToString($_, 2).PadLeft(8, '0') };
    $asBinaryStringWithPadding = [System.String]::Concat($publisherSha256AsBinary).PadRight(65, '0');
 
    $encodingTable = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
 
    $result = "";
    for ($i = 0; $i -lt $asBinaryStringWithPadding.Length; $i += 5)
    {
        $asIndex = [System.Convert]::ToInt32($asBinaryStringWithPadding.Substring($i, 5), 2);
        $result += $encodingTable[$asIndex];
    }
 
    return $result.ToLower();
}

function Extract-Macro($header, $macro)
{
	$match = Select-String -Path $header "#define $($macro) "
	$str = $match.ToString()
	$idx = $str.LastIndexOf(' ')	
	return $str.Substring($idx+1)
}
