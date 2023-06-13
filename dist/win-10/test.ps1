$basefolder  = "."
$filespath   = "./Studio/x64/Visualizations"
$filesfilter = "*"
$xmlloadpath = "Studio.wxs"
$xmlsavepath = (Resolve-Path $xmlloadpath).Path + ".mod"


Set-Location $basefolder
$files = Get-ChildItem -Path $filespath -File -Recurse -Exclude "Studio.exe" -Filter $filesfilter | 
  Select-Object -Expand FullName | 
  Resolve-Path -Relative 


$xml = [xml](Get-Content -Path $xmlloadpath)
$node = $xml.Wix.Package.ComponentGroup | where {$_.Id -eq 'ProgramFiles'}
$node = $node.Component | where {$_.Id -eq 'FILES'}

#Write-Output($node)

$i = 0
foreach ($file in $files)
{
  $plainname = (Split-Path $file -leaf)
  $testname = $file.Replace('\', '_')
  Write-Output $testname
  
  $newelement = $xml.CreateElement("File", "http://wixtoolset.org/schemas/v4/wxs")
  $newelement.SetAttribute("Id", '_' + $i)
  $newelement.SetAttribute("Source", $file)
  $node.AppendChild($newelement)
  $i++
}


$xml.save($xmlsavepath)
