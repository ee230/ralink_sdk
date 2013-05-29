<html>
<head>
<title>Operation Mode</title>
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("main");

var opmode;
var old_mode;
var hwnatb = "<% getHWNATBuilt(); %>";
var apclib = "<% getWlanApcliBuilt(); %>";
var eth_conv_mac = "<% getCfgGeneral(1, "ethConvertMAC"); %>";

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

function swtich_ethConv_apcli()
{
	document.getElementById("div_ethConvMac").style.visibility = "hidden";
	document.getElementById("div_ethConvMac").style.display = "none";
	document.getElementById("oEthConvMode").disabled = true;

	if (document.opmode.ethConv.options.selectedIndex == 1) {
		document.getElementById("div_ethConvMac").style.visibility = "visible";
		document.getElementById("div_ethConvMac").style.display = "block";
		document.getElementById("ethConv").disabled = false;
		document.getElementById("oEthConvMode").disabled = false;
		if (document.opmode.oEthConvMode[2].checked == true)
			switchEthConvMode(2);
		else if (document.opmode.oEthConvMode[1].checked == true)
			switchEthConvMode(1);
		else
			switchEthConvMode(0);
	} else if (document.opmode.apcliEnbl.options.selectedIndex == 1) {
		document.getElementById("ethConv").disabled = true;
		document.getElementById("apcliEnbl").disabled = false;
	} else {
		document.getElementById("ethConv").disabled = false;
		document.getElementById("apcliEnbl").disabled = false;
	}
}

function switchEthConvMode(mode)
{
	document.getElementById("oEthConvMac").disabled = true;
	document.opmode.oEthConvMode[mode].checked = true;
	if (mode == 2)
	{
		document.opmode.oEthConvMac.disabled = false;
		document.opmode.oEthConvMac.value = eth_conv_mac;
	}
}

function changeMode()
{
	var nat_en = "<% getCfgZero(1, "natEnabled"); %>";
	var dpbsta = "<% getDpbSta(); %>";
	var ec_en = "<% getCfgZero(1, "ethConvert"); %>";
	var mii_built = "<% getMiiInicBuilt(); %>";
	if (hwnatb == "1")
		var hwnat_en = "<% getCfgZero(1, "hwnatEnabled"); %>";
	if (apclib == "1")
		var apcli_en = "<% getCfgZero(1, "apClient"); %>";

	document.getElementById("eth_conv").style.visibility = "hidden";
	document.getElementById("eth_conv").style.display = "none";
	document.getElementById("ethConv").disabled = true;
	document.getElementById("div_apcli").style.visibility = "hidden";
	document.getElementById("div_apcli").style.display = "none";
	document.getElementById("apcliEnbl").disabled = true;
	document.getElementById("nat").style.visibility = "hidden";
	document.getElementById("nat").style.display = "none";
	document.getElementById("nat").disabled = true;
	document.opmode.tcp_timeout.disabled = true;
	document.opmode.udp_timeout.disabled = true;
	document.getElementById("div_hwnat").style.visibility = "hidden";
	document.getElementById("div_hwnat").style.display = "none";
	document.getElementById("hwnatEnbl").disabled = true;
	document.getElementById("miiInic").style.visibility = "hidden";
	document.getElementById("miiInic").style.display = "none";
	document.getElementById("miiInic").disabled = true;

	if (document.opmode.opMode[0].checked) {
		opmode = 0;
		if (dpbsta == "1") {
			document.getElementById("eth_conv").style.visibility = "visible";
			document.getElementById("eth_conv").style.display = "block";
			document.getElementById("ethConv").disabled = false;
			if (ec_en == "1") {
				document.opmode.ethConv.options.selectedIndex = 1;
				if (eth_conv_mac == "00:00:00:00:00:00" || eth_conv_mac == "") 
				{
					document.opmode.oEthConvMode[0].checked = true;
				}
				else if (eth_conv_mac == "FF:FF:FF:FF:FF:FF")
				{
					document.opmode.oEthConvMode[1].checked = true;
				}
				else
				{
					document.opmode.oEthConvMode[2].checked = true;
				}
			}
			else
			{
				document.opmode.ethConv.options.selectedIndex = 0;
			}
		}
		if (apclib == "1") {
			document.getElementById("div_apcli").style.visibility = "visible";
			document.getElementById("div_apcli").style.display = "block";
			if (apcli_en == "1" && ec_en != "1")
				document.opmode.apcliEnbl.options.selectedIndex = 1;
			else
				document.opmode.apcliEnbl.options.selectedIndex = 0;
		}
		swtich_ethConv_apcli();
		if (mii_built == "1") {
			var mii_en = "<% getCfg2Zero(1, "InicMiiEnable"); %>";

			document.getElementById("miiInic").style.visibility = "visible";
			document.getElementById("miiInic").style.display = "block";
			document.getElementById("miiInic").disabled = false;
			if (mii_en == "1") {
				document.opmode.miiMode.options.selectedIndex = 1;
			}
		}
	}
	else if (document.opmode.opMode[1].checked || document.opmode.opMode[3].checked) {
		opmode = 1;
		document.getElementById("nat").style.visibility = "visible";
		document.getElementById("nat").style.display = "block";
		document.getElementById("nat").disabled = false;
		if (nat_en == "1") {
			document.opmode.natEnbl.options.selectedIndex = 1;
		}
		switch_nat_opmode();
		if (hwnatb == "1") {
			document.getElementById("div_hwnat").style.visibility = "visible";
			document.getElementById("div_hwnat").style.display =  style_display_on();
			document.getElementById("hwnatEnbl").disabled = false;
			if (hwnat_en == "1")
				document.opmode.hwnatEnbl.options.selectedIndex = 1;
		}
		var tcp_timeout = "<% getCfgGeneral(1, "TcpTimeout"); %>";
		var udp_timeout = "<% getCfgGeneral(1, "UdpTimeout"); %>";
		if (tcp_timeout != "") 
			document.opmode.tcp_timeout.value = tcp_timeout;
		if (udp_timeout != "") 
			document.opmode.udp_timeout.value = udp_timeout;
	}
	else if (document.opmode.opMode[2].checked) {
		opmode = 2;
		//keep the nat table hidden, and always enable NAT
		document.getElementById("nat").disabled = false;
		document.opmode.natEnbl.options.selectedIndex = 1;
		if (hwnatb == "1") {
			document.getElementById("hwnatEnbl").disabled = false;
			if (hwnat_en == "1")
				document.opmode.hwnatEnbl.options.selectedIndex = 1;
		}
	}
}

function initTranslation()
{
	var e = document.getElementById("oTitle");
	e.innerHTML = _("opmode title");
	e = document.getElementById("oIntroduction");
	e.innerHTML = _("opmode introduction");

	e = document.getElementById("oModeB");
	e.innerHTML = _("opmode mode b");
	e = document.getElementById("oModeBIntro");
	e.innerHTML = _("opmode mode b intro");
	e = document.getElementById("oModeG");
	e.innerHTML = _("opmode mode g");
	e = document.getElementById("oModeGIntro");
	e.innerHTML = _("opmode mode g intro");
	e = document.getElementById("oModeE");
	e.innerHTML = _("opmode mode e");
	e = document.getElementById("stadd");
	e.innerHTML = _("opmode mode e intro");
	e = document.getElementById("oModeA");
	e.innerHTML = _("opmode mode a");
	e = document.getElementById("apclidd");
	e.innerHTML = _("opmode mode a intro");

	e = document.getElementById("oNat");
	e.innerHTML = _("opmode nat");
	e = document.getElementById("oNatD");
	e.innerHTML = _("main disable");
	e = document.getElementById("oNatE");
	e.innerHTML = _("main enable");
	e = document.getElementById("hwnat");
	e.innerHTML = _("opmode hwnat");
	e = document.getElementById("hwnatD");
	e.innerHTML = _("main disable");
	e = document.getElementById("hwnatE");
	e.innerHTML = _("main enable");

	e = document.getElementById("oEthConv");
	e.innerHTML = _("opmode eth conv");
	e = document.getElementById("oEthConvD");
	e.innerHTML = _("main disable");
	e = document.getElementById("oEthConvE");
	e.innerHTML = _("main enable");
	e = document.getElementById("apcli_mode");
	e.innerHTML = _("opmode apcli");
	e = document.getElementById("apcliD");
	e.innerHTML = _("main disable");
	e = document.getElementById("apcliE");
	e.innerHTML = _("main enable");

	e = document.getElementById("oApply");
	e.value = _("main apply");
	e = document.getElementById("oCancel");
	e.value = _("main cancel");
}

function switch_nat_opmode()
{
	document.opmode.tcp_timeout.disabled = true;
	document.opmode.udp_timeout.disabled = true;
	if (document.opmode.natEnbl.selectedIndex == 1)
	{
		document.opmode.tcp_timeout.disabled = false;
		document.opmode.udp_timeout.disabled = false;
	}
}

function initValue()
{
	opmode = "<% getCfgZero(1, "OperationMode"); %>";
	old_mode = opmode;

	var gwb = "<% getGWBuilt(); %>";
	var sta = "<% getStationBuilt(); %>";

	initTranslation();

	if (gwb == "0") {
		document.getElementById("gwdt").style.visibility = "hidden";
		document.getElementById("gwdt").style.display = "none";
		document.getElementById("oModeGIntro").style.visibility = "hidden";
		document.getElementById("oModeGIntro").style.display = "none";
	}
	if (apclib == "0") {
		document.getElementById("apclidt").style.visibility = "hidden";
		document.getElementById("apclidt").style.display = "none";
		document.getElementById("apclidd").style.visibility = "hidden";
		document.getElementById("apclidd").style.display = "none";
	}
	if (sta == "0") {
		document.getElementById("stadt").style.visibility = "hidden";
		document.getElementById("stadt").style.display = "none";
		document.getElementById("stadd").style.visibility = "hidden";
		document.getElementById("stadd").style.display = "none";
	}

	if (opmode == "1")
		document.opmode.opMode[1].checked = true;
	else if (opmode == "2")
		document.opmode.opMode[2].checked = true;
	else if (opmode == "3")
		document.opmode.opMode[3].checked = true;
	else
		document.opmode.opMode[0].checked = true;
	changeMode();
}

function checkData()
{
	if (document.opmode.oEthConvMode[2].checked == true)
	{
		var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
		if (document.opmode.oEthConvMac.value.length == 0)
		{
			alert("MAC Address should not be empty!");
			document.opmode.oEthConvMac.focus();
			return false;
		}
		if (!re.test(document.opmode.oEthConvMac.value))
		{
			alert("Please fill the MAC Address in correct format! (XX:XX:XX:XX:XX:XX)");
			document.opmode.oEthConvMac.focus();
			return false;
		}
	}

	return true;
}

function msg()
{
	if(document.opmode.opMode[1].checked == true && <% isOnePortOnly(); %> ){
		alert("In order to access web page please \nchange or alias your IP address to 172.32.1.1");
	}
}

function opmode_submit()
{
	msg();
	if (checkData() == true)
	{
		document.opmode.submit(); 
		parent.menu.location.reload();
	}
}
</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="oTitle"></h1>
<p id="oIntroduction"></p>
<hr>

<form method="post" name="opmode" action="/goform/setOpMode">
<dl>
  <dt><input type="radio" name="opMode" id="opMode" value="0" onClick="changeMode()"><b id="oModeB">Bridge:</b></dt>
  <dd id="oModeBIntro"></dd>
  <dt id="gwdt"><input type="radio" name="opMode" id="opMode" value="1" onClick="changeMode()"><b id="oModeG">Gateway:</b></dt>
  <dd id="oModeGIntro"></dd>
  <dt id="stadt"><input type="radio" name="opMode" id="opMode" value="2" onClick="changeMode()"><b id="oModeE">Ethernet Converter:</b></dt>
  <dd id="stadd"></dd>
  <dt id="apclidt"><input type="radio" name="opMode" id="opMode" value="3" onClick="changeMode()"><b id="oModeA">AP Client:</b></dt>
  <dd id="apclidd"></dd>
</dl>
<table id="nat" border="0" cellpadding="2" cellspacing="1">
<tr>
  <td id="oNat">NAT Enabled:<td>
  <td>
    <select id="natEnbl" name="natEnbl" size="1" OnChange="switch_nat_opmode()">
      <option value="0" id="oNatD">Disable</option>
      <option value="1" id="oNatE">Enable</option>
    </select>
  </td>
</tr>
<tr>
  <td>TCP Timeout:<td>
  <td><input type="text" name="tcp_timeout" size="4" maxlength="5" value="180"></td>
</tr>
<tr>
  <td>UDP Timeout:<td>
  <td><input type="text" name="udp_timeout" size="4" maxlength="5" value="180"</td>
</tr>
<tr id="div_hwnat">
  <td id="hwnat">NAT Enabled:<td>
  <td>
    <select id="hwnatEnbl" name="hwnatEnbl" size="1">
      <option value="0" id="hwnatD">Disable</option>
      <option value="1" id="hwnatE">Enable</option>
    </select>
  </td>
</tr>
</table>
<table id="eth_conv" border="0" cellpadding="2" cellspacing="1">
<tr>
  <td id="oEthConv">Ethernet Converter Enabled:<td>
  <td>
    <select id="ethConv" name="ethConv" size="1" OnChange="swtich_ethConv_apcli()">
      <option value="0" id="oEthConvD">Disable</option>
      <option value="1" id="oEthConvE">Enable</option>
    </select>
  </td>
</tr>
<tr id ="div_ethConvMac">
  <td>
    <dl>
      <dt><input type="radio" name="oEthConvMode" id="oEthConvMode" value="0" onClick="switchEthConvMode(0)">use wireless itself MAC
      <dt><input type="radio" name="oEthConvMode" id="oEthConvMode" value="1" onClick="switchEthConvMode(1)">use source MAC of first packet coming from wired device
      <dt><input type="radio" name="oEthConvMode" id="oEthConvMode" value="2" onClick="switchEthConvMode(2)">use desired MAC
      <dd><input type="text" name="oEthConvMac" id="oEthConvMac" maxlength=17 value=""></dd>
    </dl>
  </td>
</tr>
</table>
<table id="div_apcli" border="0" cellpadding="2" cellspacing="1">
<tr>
  <td id="apcli_mode">AP Client:<td>
  <td>
    <select id="apcliEnbl" name="apcliEnbl" size="1" OnChange="swtich_ethConv_apcli()">
      <option value="0" id="apcliD">Disable</option>
      <option value="1" id="apcliE">Enable</option>
    </select>
  </td>
</tr>
</table>
<table id="miiInic" border="0" cellpadding="2" cellspacing="1">
<tr>
  <td id="oMiiMode">INIC Mii Mode:<td>
  <td>
    <select id="miiMode" name="miiMode" size="1">
      <option value="0" id="oMiiModeD">Disable</option>
      <option value="1" id="oMiiModeE">Enable</option>
    </select>
  </td>
</tr>
</table>
<p />
<center>
<input type="button" style="{width:120px;}" value="Apply" id="oApply" onClick="opmode_submit();">&nbsp;&nbsp;
<input type="reset" style="{width:120px;}" value="Reset" id="oCancel" onClick="window.location.reload()">
</center>
</form>

</td></tr></table>
</body>
</html>
