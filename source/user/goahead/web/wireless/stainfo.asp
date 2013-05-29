<!-- Copyright 2004, Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<title>Station List</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

function initTranslation()
{
	var e = document.getElementById("stalistTitle");
	e.innerHTML = _("stalist title");
	e = document.getElementById("stalistIntroduction");
	e.innerHTML = _("stalist introduction");
	e = document.getElementById("stalistWirelessNet");
	e.innerHTML = _("stalist wireless network");
	e = document.getElementById("stalistMacAddr");
	e.innerHTML = _("stalist macaddr");
}

function PageInit()
{
	var txbf = "<% getTxBfBuilt(); %>";

	initTranslation();

	if (txbf == "1") {
		document.getElementById("div_info_normal").style.visibility = "hidden";
		document.getElementById("div_info_normal").style.display = "none";
	}
	else {
		document.getElementById("div_info_txbf").style.visibility = "hidden";
		document.getElementById("div_info_txbf").style.display = "none";
	}
}
</script>
</head>


<body onLoad="PageInit()">
<table class="body"><tr><td>

<h1 id="stalistTitle">Station List</h1>
<p id="stalistIntroduction"> You could monitor stations which associated to this AP here. </p>
<hr />

<table width="540" border="1" cellpadding="2" cellspacing="1">
  <tr> 
    <td class="title" id="stalistWirelessNet">Wireless Network</td>
  </tr>
</table>
<table id="div_info_normal" width="540" border="1" cellpadding="2" cellspacing="1">
  <tr>
    <td bgcolor=#E8F8FF id="stalistMacAddr">MAC Address</td>
    <td bgcolor=#E8F8FF>Aid</td>
    <td bgcolor=#E8F8FF>PSM</td>
    <td bgcolor=#E8F8FF>MimoPS</td>
    <td bgcolor=#E8F8FF>MCS</td>
    <td bgcolor=#E8F8FF>BW</td>
    <td bgcolor=#E8F8FF>SGI</td>
    <td bgcolor=#E8F8FF>STBC</td>
  </tr>
  <% getWlanStaInfo(); %>
</table>
<table id="div_info_txbf" width="540" border="1" cellpadding="2" cellspacing="1">
  <tr>
    <td bgcolor=#E8F8FF id="stalistMacAddr">MAC Addr</td>
    <td bgcolor=#E8F8FF>Aid</td>
    <td bgcolor=#E8F8FF>PSM</td>
    <td bgcolor=#E8F8FF>MIMO<br>PS</td>
    <td bgcolor=#E8F8FF>TX Rate</td>
    <td bgcolor=#E8F8FF>TxBF</td>
    <td bgcolor=#E8F8FF>RSSI</td>
    <td bgcolor=#E8F8FF>Stream<br>SNR</td>
    <td bgcolor=#E8F8FF>Snd Rsp<br>SNR</td>
    <td bgcolor=#E8F8FF>Last<br>RX Rate</td>
    <td bgcolor=#E8F8FF>Connect<br>Time</td>
  </tr>
  <% getWlanStaInfo(); %>
</table>

</td></tr></table>
</body>
</html>

