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
    [System.Xml.XmlNode] $dirnode,
    [System.Boolean] $rootfolder 
  )

  $newguid = [guid]::NewGuid()
  $newcompid = ("-" + $newguid).Replace('-', '_')
  
  $newcomponent = $xml.CreateElement("Component", "http://wixtoolset.org/schemas/v4/wxs")
  $newcomponent.SetAttribute("Id", $newcompid)
  $newcomponent.SetAttribute("Guid", $newguid)
  
  $contents = (Get-ChildItem -Path $path)
  foreach ($c in $contents)
  {
    $plainname = (Split-Path $c -leaf)

    if ($c -Is [System.IO.DirectoryInfo])
    {
      #Write-Output($c.FullName)
      #$plainname = (Split-Path $c -leaf)

      $newfolder = $xml.CreateElement("Directory", "http://wixtoolset.org/schemas/v4/wxs")
      #$newfolder.SetAttribute("Id", '_' + $i)
      $newfolder.SetAttribute("Name", $plainname)
      $dirnode.AppendChild($newfolder)

      process $c.FullName $newfolder 0
    }
    else
    {     
      $newfile = $xml.CreateElement("File", "http://wixtoolset.org/schemas/v4/wxs")
      $newfile.SetAttribute("Source", (Resolve-Path -Relative $c.FullName))
      
      $ext = [System.IO.Path]::GetExtension($c)
      if ($ext -eq ".exe")
      {
         if ($rootfolder) {
           $newfile.SetAttribute("Id", $plainname)
         }

         $newrule = $xml.CreateElement("fire:FirewallException", "http://wixtoolset.org/schemas/v4/wxs/firewall")             
         #$newrule.SetAttribute("Id", "private")
         $newrule.SetAttribute("Name", $plainname)        
         $newrule.SetAttribute("Profile", "private")
         $newrule.SetAttribute("Scope", "any")
         $newfile.AppendChild($newrule)
      }     
      $newcomponent.AppendChild($newfile)
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

process $filespath $node 1

$xml.save($xmlsavepath)
