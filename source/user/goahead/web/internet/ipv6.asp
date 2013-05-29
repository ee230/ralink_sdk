<html>
<head>
<title>IPv6 Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("internet");
var ipv66rdb = "<% getIPv66rdBuilt(); %>";
var ipv6dsb = "<% getIPv6DSBuilt(); %>";

function display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Firefox, Safari,...
		return "table-row";
	}
}

function initTranslation()
{
}

function SwitchOpMode()
{
	document.getElementById("v6StaticTable").style.visibility = "hidden";
	document.getElementById("v6StaticTable").style.display = "none";
	document.ipv6_cfg.ipv6_lan_ipaddr.disabled = true;
	document.ipv6_cfg.ipv6_lan_prefix_len.disabled = true;
	document.ipv6_cfg.ipv6_wan_ipaddr.disabled = true;
	document.ipv6_cfg.ipv6_wan_prefix_len.disabled = true;
	document.ipv6_cfg.ipv6_static_gw.disabled = true;
	document.getElementById("v66rdTable").style.visibility = "hidden";
	document.getElementById("v66rdTable").style.display = "none";
	document.ipv6_cfg.ipv6_6rd_prefix.disabled = true;
	document.ipv6_cfg.ipv6_6rd_prefix_len.disabled = true;
	document.ipv6_cfg.ipv6_6rd_border_ipaddr.disabled = true;
	document.getElementById("v6DSTable").style.visibility = "hidden";
	document.getElementById("v6DSTable").style.display = "none";
	document.ipv6_cfg.ipv6_ds_wan_ipaddr.disabled = true;
	document.ipv6_cfg.ipv6_ds_aftr_ipaddr.disabled = true;
	document.ipv6_cfg.ipv6_ds_gw_ipaddr.disabled = true;

	if (document.ipv6_cfg.ipv6_opmode.options.selectedIndex == 1) {
		document.getElementById("v6StaticTable").style.visibility = "visible";
		document.getElementById("v6StaticTable").style.display = display_on();
		document.ipv6_cfg.ipv6_lan_ipaddr.disabled = false;
		document.ipv6_cfg.ipv6_lan_prefix_len.disabled = false;
		document.ipv6_cfg.ipv6_wan_ipaddr.disabled = false;
		document.ipv6_cfg.ipv6_wan_prefix_len.disabled = false;
		document.ipv6_cfg.ipv6_static_gw.disabled = false;
	} else if (ipv66rdb == "1" && document.ipv6_cfg.ipv6_opmode.options.selectedIndex == 2) {
		document.getElementById("v66rdTable").style.visibility = "visible";
		document.getElementById("v66rdTable").style.display = display_on();
		document.ipv6_cfg.ipv6_6rd_prefix.disabled = false;
		document.ipv6_cfg.ipv6_6rd_prefix_len.disabled = false;
		document.ipv6_cfg.ipv6_6rd_border_ipaddr.disabled = false;
	} else if (document.ipv6_cfg.ipv6_opmode.options.selectedIndex == document.ipv6_cfg.ipv6_opmode.options.length-1) {
		document.getElementById("v6DSTable").style.visibility = "visible";
		document.getElementById("v6DSTable").style.display = display_on();
		document.ipv6_cfg.ipv6_ds_wan_ipaddr.disabled = false;
		document.ipv6_cfg.ipv6_ds_aftr_ipaddr.disabled = false;
		document.ipv6_cfg.ipv6_ds_gw_ipaddr.disabled = false;
	}
}

function initValue()
{
	var opmode = "<% getCfgZero(1, "IPv6OpMode"); %>";
	var opmode_len = document.ipv6_cfg.ipv6_opmode.options.length;

	if (ipv66rdb == "1") {
		document.ipv6_cfg.ipv6_opmode.options[2] = new Option("Tunneling Connection (6RD)", "2");
		opmode_len++;
	}
	if (ipv6dsb == "1") {
		document.ipv6_cfg.ipv6_opmode.options[opmode_len] = new Option("Tunneling Connection (DS-Lite)", "3");
		opmode_len++;
	}

	if (opmode == "1")
		document.ipv6_cfg.ipv6_opmode.options.selectedIndex = 1;
	else if (opmode == "2")
		document.ipv6_cfg.ipv6_opmode.options.selectedIndex = 2;
	else if (opmode == "3")
		document.ipv6_cfg.ipv6_opmode.options.selectedIndex = opmode_len-1;

	SwitchOpMode();

	if (opmode == "1") {
		document.ipv6_cfg.ipv6_lan_ipaddr.value = "<% getCfgGeneral(1, "IPv6IPAddr"); %>";
		document.ipv6_cfg.ipv6_lan_prefix_len.value = "<% getCfgGeneral(1, "IPv6PrefixLen"); %>";
		document.ipv6_cfg.ipv6_wan_ipaddr.value = "<% getCfgGeneral(1, "IPv6WANIPAddr"); %>";
		document.ipv6_cfg.ipv6_wan_prefix_len.value = "<% getCfgGeneral(1, "IPv6WANPrefixLen"); %>";
		document.ipv6_cfg.ipv6_static_gw.value = "<% getCfgGeneral(1, "IPv6GWAddr"); %>";
	} else if (opmode == "2") {
		document.ipv6_cfg.ipv6_6rd_prefix.value = "<% getCfgGeneral(1, "IPv6IPAddr"); %>";
		document.ipv6_cfg.ipv6_6rd_prefix_len.value = "<% getCfgGeneral(1, "IPv6PrefixLen"); %>";
		document.ipv6_cfg.ipv6_6rd_border_ipaddr.value = "<% getCfgGeneral(1, "IPv6SrvAddr"); %>";
	} else if (opmode == "3") {
		document.ipv6_cfg.ipv6_ds_wan_ipaddr.value = "<% getCfgGeneral(1, "IPv6IPAddr"); %>";
		document.ipv6_cfg.ipv6_ds_aftr_ipaddr.value = "<% getCfgGeneral(1, "IPv6SrvAddr"); %>";
		document.ipv6_cfg.ipv6_ds_gw_ipaddr.value = "<% getCfgGeneral(1, "IPv6GWAddr"); %>";
	}
}

function atoi(str, num)
{
	i = 1;
	if (num != 1) {
		while (i != num && str.length != 0) {
			if (str.charAt(0) == '.') {
				i++;
			}
			str = str.substring(1);
		}
		if (i != num)
			return -1;
	}

	for (i=0; i<str.length; i++) {
		if (str.charAt(i) == '.') {
			str = str.substring(0, i);
			break;
		}
	}
	if (str.length == 0)
		return -1;
	return parseInt(str, 10);
}

function checkRange(str, num, min, max)
{
	d = atoi(str, num);
	if (d > max || d < min)
		return false;
	return true;
}

function isAllNum(str)
{
	for (var i=0; i<str.length; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
}

function checkIpv4Addr(field)
{
	if (field.value == "") {
		alert("Error. IP address is empty.");
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if (isAllNum(field.value) == 0) {
		alert('It should be a [0-9] number.');
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if ((!checkRange(field.value, 1, 0, 255)) ||
			(!checkRange(field.value, 2, 0, 255)) ||
			(!checkRange(field.value, 3, 0, 255)) ||
			(!checkRange(field.value, 4, 1, 254)))
	{
		alert('IP adress format error.');
		field.value = field.defaultValue;
		field.focus();
		return false;
	}
	return true;
}

function checkIpv6Addr(ip_addr, len)
{
	var ip_item = new Array();
	ip_item = ip_addr.split(":");
	for (var i=0; i<ip_item.length; i++) {
		if (parseInt(ip_item[i], 16) == "NaN") {
			alert('It should be a [0-F] number.');
			return false;
		}
	}

	return true;
}

function CheckValue()
{
	if (document.ipv6_cfg.ipv6_opmode.value == "1") {
		if (document.ipv6_cfg.ipv6_lan_ipaddr.value == "" &&
		    document.ipv6_cfg.ipv6_wan_ipaddr.value == "") {
			alert("please fill LAN/WAN IPv6 Address!");
			return false;
		}
		if (document.ipv6_cfg.ipv6_lan_ipaddr.value != "") {
			if (!checkIpv6Addr(document.ipv6_cfg.ipv6_lan_ipaddr.value, 128)) {
				alert("invalid IPv6 IP address!");
				document.ipv6_cfg.ipv6_lan_ipaddr.focus();
				document.ipv6_cfg.ipv6_lan_ipaddr.select();
				return false;
			}
			if (document.ipv6_cfg.ipv6_lan_prefix_len.value == "" || 
			    document.ipv6_cfg.ipv6_lan_prefix_len.value > 128 || 
			    document.ipv6_cfg.ipv6_lan_prefix_len.value < 0) {
				alert("invalid prefix length!");
				document.ipv6_cfg.ipv6_lan_prefix_len.focus();
				document.ipv6_cfg.ipv6_lan_prefix_len.select();
				return false;
			}
		}
		if (document.ipv6_cfg.ipv6_wan_ipaddr.value != "") {
			if (!checkIpv6Addr(document.ipv6_cfg.ipv6_wan_ipaddr.value, 128)) {
				alert("invalid IPv6 IP address!");
				document.ipv6_cfg.ipv6_wan_ipaddr.focus();
				document.ipv6_cfg.ipv6_wan_ipaddr.select();
				return false;
			}
			if (document.ipv6_cfg.ipv6_wan_prefix_len.value == "" ||
			    document.ipv6_cfg.ipv6_wan_prefix_len.value > 128 || 
			    document.ipv6_cfg.ipv6_wan_prefix_len.value < 0) {
				alert("invalid prefix length!");
				document.ipv6_cfg.ipv6_wan_prefix_len.focus();
				document.ipv6_cfg.ipv6_wan_prefix_len.select();
				return false;
			}
		}
		if (document.ipv6_cfg.ipv6_static_gw.value != "" &&
		    (!checkIpv6Addr(document.ipv6_cfg.ipv6_static_gw.value, 128))) {
			alert("invalid IPv6 IP address!");
			document.ipv6_cfg.ipv6_static_gw.focus();
			document.ipv6_cfg.ipv6_static_gw.select();
			return false;
		}
	} else if (document.ipv6_cfg.ipv6_opmode.value == "2") {
		if (document.ipv6_cfg.ipv6_6rd_prefix.value == "" ||
		    document.ipv6_cfg.ipv6_6rd_prefix_len.value == "" ||
		    document.ipv6_cfg.ipv6_6rd_border_ipaddr.value == "") {
			alert("please fill all fields!");
			return false;
		}
		if (!checkIpv6Addr(document.ipv6_cfg.ipv6_6rd_prefix.value, 32)) {
			alert("invalid IPv6 IP address!");
			document.ipv6_cfg.ipv6_6rd_prefix.focus();
			document.ipv6_cfg.ipv6_6rd_prefix.select();
			return false;
		}
		if (document.ipv6_cfg.ipv6_6rd_prefix_len.value > 32 || 
		    document.ipv6_cfg.ipv6_6rd_prefix_len.value < 0) {
			alert("invalid prefix length!");
			document.ipv6_cfg.ipv6_6rd_prefix_len.focus();
			document.ipv6_cfg.ipv6_6rd_prefix_len.select();
			return false;
		}
		if (!checkIpv4Addr(document.ipv6_cfg.ipv6_6rd_border_ipaddr.value)) {
			alert("invalid IPv4 ip address!");
			document.ipv6_cfg.ipv6_6rd_border_ipaddr.focus();
			document.ipv6_cfg.ipv6_6rd_border_ipaddr.select();
			return false;
		}
	} else if (document.ipv6_cfg.ipv6_opmode.value == "3") {
		if (document.ipv6_cfg.ipv6_ds_wan_ipaddr.value == "" ||
		    document.ipv6_cfg.ipv6_ds_aftr_ipaddr.value == "" ||
		    document.ipv6_cfg.ipv6_ds_gw_ipaddr.value == "") {
			alert("please fill all fields!");
			return false;
		}
		if (!checkIpv6Addr(document.ipv6_cfg.ipv6_ds_wan_ipaddr.value, 128)) {
			alert("invalid IPv6 ip address!");
			document.ipv6_cfg.ipv6_ds_wan_ipaddr.focus();
			document.ipv6_cfg.ipv6_ds_wan_ipaddr.select();
			return false;
		}
		if (!checkIpv6Addr(document.ipv6_cfg.ipv6_ds_aftr_ipaddr.value, 128)) {
			alert("invalid IPv6 ip address!");
			document.ipv6_cfg.ipv6_ds_aftr_ipaddr.focus();
			document.ipv6_cfg.ipv6_ds_aftr_ipaddr.select();
			return false;
		}
		if (!checkIpv6Addr(document.ipv6_cfg.ipv6_ds_gw_ipaddr.value, 128)) { 
			alert("invalid ipv6 ip address!");
			document.ipv6_cfg.ipv6_ds_gw_ipaddr.focus();
			document.ipv6_cfg.ipv6_ds_gw_ipaddr.select();
			return false;
		}
	}
	
	return true;
}
</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="lTitle">
<p id="lIntroduction">IPv6 Setup</p>
</h1>
<hr />

<form method=post name="ipv6_cfg" action="/goform/setIPv6" onSubmit="return CheckValue()">
<table width="95%" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="v6ConnType">IPv6 Connection Type</td>
</tr>
<tr>
  <td class="head" id="v6OpMode">IPv6 Operation Mode</td>
  <td>
    <select name="ipv6_opmode" size="1" onChange="SwitchOpMode()">
      <option value="0" id="v6Disable">Disable</option>
      <option value="1" id="v6Static">Static IP Connection</option>
    </select>
  </td>
</tr>
</table>
<!-- STATIC IP --!>
<table width="95%" id="v6StaticTable" border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2" width="540" style="visibility: hidden;">
<tr>
  <td class="title" colspan="2" id="v6StaticIPSetup">IPv6 Static IP Setup</td>
</tr>
<tr>
  <td class="head" id="v6StaticIPAddr">LAN IPv6 Address / Subnet Prefix Length</td>
  <td><input name="ipv6_lan_ipaddr" maxlength=39 size=27> / <input name="ipv6_lan_prefix_len" maxlength=3 size=2></td>
</tr>
<tr>
  <td class="head" id="v6StaticIPAddr">WAN IPv6 Address / Subnet Prefix Length</td>
  <td><input name="ipv6_wan_ipaddr" maxlength=39 size=27> / <input name="ipv6_wan_prefix_len" maxlength=3 size=2></td>
</tr>
<tr>
  <td class="head" id="v6StaticGW">Default Gateway</td>
  <td><input name="ipv6_static_gw" maxlength=39 size=27></td>
</tr>
</table>
<!-- 6RD --!>
<table width="95%" id="v66rdTable" border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2" width="540" style="visibility: hidden;">
<tr>
  <td class="title" colspan="2" id="v66rdSetup">Tunneling Connection (6RD) Setup</td>
</tr>
<tr>
  <td class="head" id="v66rdPrefix">ISP 6rd Prefix / Prefix Length</td>
  <td><input name="ipv6_6rd_prefix" maxlength=9 size=7> / <input name="ipv6_6rd_prefix_len" maxlength=3 size=2></td>
</tr>
<tr>
  <td class="head" id="v66rdBorderIPAddr">ISP Border Relay IPv4 Address</td>
  <td><input name="ipv6_6rd_border_ipaddr" maxlength=15 size=13></td>
</tr>
</table>
<!-- DS-Lite --!>
<table width="95%" id="v6DSTable" border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2" width="540" style="visibility: hidden;">
<tr>
  <td class="title" colspan="2" id="v6DSSetup">Tunneling Connection (DS-Lite) Setup</td>
</tr>
<tr>
  <td class="head" id="v6DSLocalIPAddr"> WAN IPv6 Address </td>
  <td><input name="ipv6_ds_wan_ipaddr" maxlength=39 size=27></td>
</tr>
<tr>
  <td class="head" id="v6DSAFTRIPAddr"> AFTR Server IPv6 Address</td>
  <td><input name="ipv6_ds_aftr_ipaddr" maxlength=39 size=27></td>
</tr>
<tr>
  <td class="head" id="v6DSGWIPAddr"> Gateway IPv6 Address </td>
  <td><input name="ipv6_ds_gw_ipaddr" maxlength=39 size=27></td>
</tr>
</table>
<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
  <td>
    <input type=submit style="{width:120px;}" value="Apply" id="v6Apply">&nbsp;&nbsp;
    <input type=reset  style="{width:120px;}" value="Cancel" id="v6Cancel">
  </td>
</tr>
</table>
</form>
</td></tr></table>
</body>
</html>

