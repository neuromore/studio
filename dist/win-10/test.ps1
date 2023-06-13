$basefolder  = "."
$filespath   = ".\Studio\x64"
$filesfilter = "*"
$xmlloadpath = "Studio.wxs"
$xmlsavepath = (Resolve-Path $xmlloadpath).Path + ".mod"

#Set-Location $basefolder

# Load WXS
$xml = [xml](Get-Content -Path $xmlloadpath)
$featurenode = $xml.Wix.Package.Feature

$node = $xml.Wix.Package.StandardDirectory | where {$_.Id -eq 'ProgramFiles6432Folder'}
$node = $node.Directory | where {$_.Id -eq 'INSTALLFOLDER'}

function process
{
  [CmdletBinding()]
  param(
    [Parameter()]
    [System.String] $path,
    [System.Xml.XmlNode] $dirnode
  )

  $newguid = [guid]::NewGuid()
  $newcompid = ("-" + $newguid).Replace('-', '_')
  
  $newcomponent = $xml.CreateElement("Component", "http://wixtoolset.org/schemas/v4/wxs")
  $newcomponent.SetAttribute("Id", $newcompid)
  $newcomponent.SetAttribute("Guid", $newguid)
  
  $contents = Get-ChildItem -Path $path
  foreach ($c in $contents)
  {
    if ($c -Is [System.IO.DirectoryInfo])
    {
      Write-Output($c.FullName)

      $plainname = (Split-Path $c -leaf)

      

      $newfolder = $xml.CreateElement("Directory", "http://wixtoolset.org/schemas/v4/wxs")
      #$newfolder.SetAttribute("Id", '_' + $i)
      $newfolder.SetAttribute("Name", $plainname)
      $dirnode.AppendChild($newfolder)

      process $c.FullName $newfolder
    }
    else
    {
      
      $newfile = $xml.CreateElement("File", "http://wixtoolset.org/schemas/v4/wxs")
      #$newfile.SetAttribute("Id", '_' + $i)
      $newfile.SetAttribute("Source", (Resolve-Path -Relative $c.FullName))
       
      $newcomponent.AppendChild($newfile)

      Write-Output($c.FullName)
    }
  }
  if ($newcomponent.ChildNodes.Count)
  {
    $dirnode.AppendChild($newcomponent)

    $newcompref = $xml.CreateElement("ComponentRef", "http://wixtoolset.org/schemas/v4/wxs")
    $newcompref.SetAttribute("Id", $newcompid)
    $featurenode.AppendChild($newcompref)
  }
}





process $filespath $node



#Set-Location $basefolder
#$files = Get-ChildItem -Path $filespath -Recurse -Exclude "Studio.exe" -Filter $filesfilter
# | 
#  Select-Object -Expand FullName | 
#  Resolve-Path -Relative 


#foreach ($file in $files)
#{
#  $item = Get-Item $file
#  if ($file -Is [System.IO.DirectoryInfo])
#  {
#    Write-Output($file)
#  }
#}

#$node = $xml.Wix.Package.ComponentGroup | where {$_.Id -eq 'ProgramFiles'}
#$node = $node.Component | where {$_.Id -eq 'FILES'}

$i = 0
#foreach ($file in $files)
#{
#  $newelement = $xml.CreateElement("File", "http://wixtoolset.org/schemas/v4/wxs")
#  $newelement.SetAttribute("Id", '_' + $i)
#  $newelement.SetAttribute("Source", $file)
#  $node.AppendChild($newelement)
#  $i++
#}


$xml.save($xmlsavepath)
