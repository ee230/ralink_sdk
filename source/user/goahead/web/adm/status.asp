<html>
<head>
<title>Access Point Status</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<META HTTP-EQUIV="refresh" CONTENT="3; URL=./status.asp">

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");

function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}

function showOpMode()
{
	var opmode = 1* <% getCfgZero(1, "OperationMode"); %>;
	if (opmode == 0)
		document.write("Bridge Mode");
	else if (opmode == 1)
		document.write("Gateway Mode");
	else if (opmode == 2)
		document.write("Ethernet Converter Mode");
	else if (opmode == 3)
		document.write("AP Client Mode");
	else
		document.write("Unknown");
}

function showPortStatus()
{
	var str = "<% getPortStatus(); %>";
	var all = new Array();

	if(str == "-1"){
		document.write("not support");
		return ;
	}

	all = str.split(",");
	for(i=0; i< all.length-1; i+=3){
		document.write("<td>");
		if(all[i] == "1"){
			if(all[i+1] == "10")
				document.write("<img src=/graphics/10.gif> ");
			else if(all[i+1] == "100")
				document.write("<img src=/graphics/100.gif> ");

//			if(all[i+2] == "F")
//				document.write("Full ");
//			else(all[i+2] == "H")
//				document.write("Half ");
		}else if(all[i] == "0"){
				document.write("<img src=/graphics/empty.gif> ");
		}
		document.write("</td>");
	}
}

function initTranslation()
{
	var e = document.getElementById("statusTitle");
	e.innerHTML = _("status title");
	e = document.getElementById("statusIntroduction");
	e.innerHTML = _("status introduction");
	
	e = document.getElementById("statusSysInfo");
	e.innerHTML = _("status system information");
	e = document.getElementById("statusSDKVersion");
	e.innerHTML = _("status sdk version");
	e = document.getElementById("statusSysUpTime");
	e.innerHTML = _("status system up time");
	e = document.getElementById("statusSysPlatform");
	e.innerHTML = _("status system platform");
	e = document.getElementById("statusOPMode");
	e.innerHTML = _("status operate mode");

	e = document.getElementById("statusInternetConfig");
	e.innerHTML = _("status internet config");
	e = document.getElementById("statusConnectedType");
	e.innerHTML = _("status connect type");
	e = document.getElementById("statusWANIPAddr");
	e.innerHTML = _("status wan ipaddr");
	e = document.getElementById("statusSubnetMask");
	e.innerHTML = _("status subnet mask");
	e = document.getElementById("statusDefaultGW");
	e.innerHTML = _("status default gateway");
	e = document.getElementById("statusPrimaryDNS");
	e.innerHTML = _("status primary dns");
	e = document.getElementById("statusSecondaryDNS");
	e.innerHTML = _("status secondary dns");
	e = document.getElementById("statusWANMAC");
	e.innerHTML = _("status mac");

	e = document.getElementById("statusLocalNet");
	e.innerHTML = _("status local network");
	e = document.getElementById("statusLANIPAddr");
	e.innerHTML = _("status lan ipaddr");
	e = document.getElementById("statusLocalNetmask");
	e.innerHTML = _("status local netmask");
	e = document.getElementById("statusLANMAC");
	e.innerHTML = _("status mac");

	e = document.getElementById("statusEthPortStatus");
	e.innerHTML = _("status ethernet port status");
}

function PageInit()
{
	var ethtoolb = "<% getETHTOOLBuilt(); %>";
	initTranslation();

	if (ethtoolb == "1")
	{
		//document.getElementById("statusEthPortStatus").style.visibility = "visible";
		//document.getElementById("statusEthPortStatus").style.display = style_display_on();
		document.getElementById("div_ethtool").style.visibility = "visible";
		document.getElementById("div_ethtool").style.display = style_display_on();
	}
	else
	{
		//document.getElementById("statusEthPortStatus").style.visibility = "hidden";
		//document.getElementById("statusEthPortStatus").style.display = "none";
		document.getElementById("div_ethtool").style.visibility = "hidden";
		document.getElementById("div_ethtool").style.display = "none";
	}
}
</script>
</head>

<body onload="PageInit()">
<table class="body"><tr><td>
<H1 id="statusTitle">Access Point Status</H1>
<P id="statusIntroduction">Let's take a look at the status of Ralink SoC Platform. </P>

<table width="95%" border="1" cellpadding="2" cellspacing="1">
<!-- ================= System Info ================= -->
<tr>
  <td class="title" colspan="2" id="statusSysInfo">System Info</td>
</tr>
<tr>
  <td class="head" id="statusSDKVersion">SDK Version</td>
  <td><% getSdkVersion(); %> (<% getSysBuildTime(); %>)</td>
</tr>
<tr>
  <td class="head" id="statusSysUpTime">System Up Time</td>
  <td><% getSysUptime(); %></td>
</tr>
<tr>
  <td class="head" id="statusSysPlatform">System Platform</td>
  <td><% getPlatform(); %></td>
</tr>
<tr>
  <td class="head" id="statusOPMode">Operation Mode</td>
  <td><script type="text/javascript">showOpMode();</script></td>
</tr>
<!-- ================= Internet Configurations ================= -->
<tr>
  <td class="title" colspan="2" id="statusInternetConfig">Internet Configurations</td>
</tr>
<tr>
  <td class="head" id="statusConnectedType">Connected Type</td>
  <td><% getCfgGeneral(1, "wanConnectionMode"); %> </td>
</tr>
<tr>
  <td class="head" id="statusWANIPAddr">WAN IP Address</td>
  <td><% getWanIp(); %></td>
</tr>
<tr>
  <td class="head" id="statusSubnetMask">Subnet Mask</td>
  <td><% getWanNetmask(); %></td>
</tr>
<tr>
  <td class="head" id="statusDefaultGW">Default Gateway</td>
  <td><% getWanGateway(); %></td>
</tr>
<tr>
  <td class="head" id="statusPrimaryDNS">Primary Domain Name Server</td>
  <td><% getDns(1); %></td>
</tr>
<tr>
  <td class="head" id="statusSecondaryDNS">Secondary Domain Name Server</td>
  <td><% getDns(2); %></td>
</tr>
<tr>
  <td class="head" id="statusWANMAC">MAC Address</td>
  <td><% getWanMac(); %></td>
</tr>
<!-- ================= Local Network ================= -->
<tr>
  <td class="title" colspan="2" id="statusLocalNet">Local Network</td>
</tr>
<tr>
  <td class="head" id="statusLANIPAddr">Local IP Address</td>
  <td><% getLanIp(); %></td>
</tr>
<tr>
  <td class="head" id="statusLocalNetmask">Local Netmask</td>
  <td><% getLanNetmask(); %></td>
</tr>
<tr>
  <td class="head" id="statusLANMAC">MAC Address</td>
  <td><% getLanMac(); %></td>
</tr>
<!-- ================= Other Information ================= -->
</table>


<table border="0" id="div_ethtool">
<tr>
  <td>
    <H1 id="statusEthPortStatus">Ethernet Port Status</H1>
  </td>
</tr>
<tr>
  <td>
    <script type="text/javascript">showPortStatus();</script>
  </td>
</tr>
</table>

</td></tr></table>
</body>
</html>
