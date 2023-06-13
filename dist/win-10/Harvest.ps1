param (
  [Parameter(Mandatory)] [System.String] $in,
  [Parameter(Mandatory)] [System.String] $out,
  [Parameter(Mandatory)] [System.String] $path
)

# Load WXS
$xml = [xml](Get-Content -Path $in)
$featurenode = $xml.Wix.Package.Feature

###############################################################################################

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
      $newfolder = $xml.CreateElement("Directory", "http://wixtoolset.org/schemas/v4/wxs")
      $newfolder.SetAttribute("Name", $plainname)
      $dirnode.AppendChild($newfolder) | Out-Null

      process $c.FullName $newfolder 0
    }
    else
    {     
      $newfile = $xml.CreateElement("File", "http://wixtoolset.org/schemas/v4/wxs")
#      $newfile.SetAttribute("Source", (Resolve-Path -Relative $c.FullName))
      $newfile.SetAttribute("Source", $c.FullName)

      $ext = [System.IO.Path]::GetExtension($c)
      if ($ext -eq ".exe") {
         if ($rootfolder) {
           $newfile.SetAttribute("Id", $plainname)
         }
         $newrule = $xml.CreateElement(
           "fire:FirewallException", 
           "http://wixtoolset.org/schemas/v4/wxs/firewall")             
         $newrule.SetAttribute("Name", $plainname)        
         $newrule.SetAttribute("Profile", "private")
         $newrule.SetAttribute("Scope", "any")
         $newfile.AppendChild($newrule) | Out-Null
      }     
      $newcomponent.AppendChild($newfile) | Out-Null
    }
  }
  if ($newcomponent.ChildNodes.Count)
  {
    $dirnode.AppendChild($newcomponent) | Out-Null
    $newcompref = $xml.CreateElement("ComponentRef", "http://wixtoolset.org/schemas/v4/wxs")
    $newcompref.SetAttribute("Id", $newcompid)
    $featurenode.AppendChild($newcompref) | Out-Null
  }
}

###############################################################################################

$node = $xml.Wix.Package.StandardDirectory | where {$_.Id -eq 'ProgramFiles6432Folder'}
$node = $node.Directory | where {$_.Id -eq 'INSTALLFOLDER'}

# Modify WXS
process $path $node 1

# Save WXS
$xml.save($out)
