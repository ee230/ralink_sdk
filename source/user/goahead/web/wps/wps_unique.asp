<html><head><title>WPS</title>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<style type="text/css">
#wps_progress_bar {
width:250px;
height:15;
margin: 0 auto;
border: 1px solid gray;
}
</style>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/wps/wps_timer.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");
var wps_status;
var wps_result;

function style_display_on(){
    if(window.ActiveXObject) { // IE
        return "block";
    } else if (window.XMLHttpRequest) { // Mozilla, Safari,...
        return "table-row";
    }
}

var http_request = false;
function makeRequest(url, content) {
	http_request = false;
	if (window.XMLHttpRequest) { // Mozilla, Safari,...
		http_request = new XMLHttpRequest();
		if (http_request.overrideMimeType) {
			http_request.overrideMimeType('text/xml');
		}
	} else if (window.ActiveXObject) { // IE
		try {
			http_request = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
			http_request = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e) {}
		}
	}
	if (!http_request) {
		alert('Giving up :( Cannot create an XMLHTTP instance');
		return false;
	}
	http_request.onreadystatechange = alertContents;
	http_request.open('POST', url, true);
	http_request.send(content);
}

function alertContents() {
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			WPSUpdateHTML(http_request.responseText);
		} else {
			alert('There was a problem with the request.');
		}
	}
}

function WPSUpdateHTML(str)
{
	var all_str = new Array();
	all_str = str.split("\n");

	wpsconfigured = document.getElementById("WPSConfigured");
	if(all_str[0] == "1" || all_str[0] == "0")
		wpsconfigured.innerHTML = "No";
	else if(all_str[0] == "2")
		wpsconfigured.innerHTML = "Yes";
	else
		wpsconfigured.innerHTML = "Unknown";
	
	wpsssid = document.getElementById("WPSSSID");
	wpsssid.innerHTML = all_str[1];

	wpsauthmode = document.getElementById("WPSAuthMode");
	wpsauthmode.innerHTML = all_str[2];

	wpsencryptype = document.getElementById("WPSEncryptype");
	wpsencryptype.innerHTML = all_str[3];

	wpsdefaultkeyindex = document.getElementById("WPSDefaultKeyIndex");
	wpsdefaultkeyindex.innerHTML = all_str[4];

	wpskeytype = document.getElementById("WPSKeyType");
	if(all_str[3] == "WEP"){
		wpskeytype.innerHTML = "WPS Key(Hex value)";
	}else
		wpskeytype.innerHTML = "WPS Key(ASCII)";

	wpswpakey = document.getElementById("WPSWPAKey");
	wpswpakey.innerHTML = all_str[5];

	wpsstatus = document.getElementById("WPSCurrentStatus");
	wpsstatus.innerHTML = all_str[6];

	if(all_str[7] == "-1")
		document.getElementById("WPSInfo").value = "WSC failed";
	else if(all_str[7] == "0"){
		document.getElementById("WPSInfo").value = "WSC:" + all_str[6];
	}else if(all_str[7] == "1")
		document.getElementById("WPSInfo").value = "WSC Success";
	wps_result = all_str[7];
	wps_status = all_str[8];
}

function updateWPS(){
	clear_progress_bar();
	makeRequest("/goform/updateWPS", "something");
	if (top.menu.isMenuLock == 1) {
		document.WPSConfig.submitWPSEnable.disabled = true;
		document.WPSConfig.WPSEnable.disabled = true;
		document.SubmitGenPIN.GenPIN.disabled = true;
		document.SubmitOOB.submitResetOOB.disabled = true;
		document.WPS.PINPBCRadio[0].disabled = true;
		document.WPS.PINPBCRadio[1].disabled = true;
		document.WPS.PIN.disabled = true;
		document.WPS.submitWPS.disabled = true;
		document.wps_cancel.wpsCancel.disabled = false;
		if (wps_result == "1" || wps_result == "-1") {
			top.menu.setLockMenu(0);
			window.location.reload();
		}
	} else {
		document.WPSConfig.submitWPSEnable.disabled = false;
		document.WPSConfig.WPSEnable.disabled = false;
		document.SubmitGenPIN.GenPIN.disabled = false;
		document.SubmitOOB.submitResetOOB.disabled = false;
		document.WPS.PINPBCRadio[0].disabled = false;
		document.WPS.PINPBCRadio[1].disabled = false;
		document.WPS.PIN.disabled = false;
		document.WPS.submitWPS.disabled = false;
		document.wps_cancel.wpsCancel.disabled = true;
	}
	if (wps_result == "1")
		refresh_progress_bar(34);
	else
		refresh_progress_bar(wps_status);
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function ValidateChecksum(PIN)
{
    var accum = 0;

    accum += 3 * (parseInt(PIN / 10000000) % 10);
    accum += 1 * (parseInt(PIN / 1000000) % 10);
    accum += 3 * (parseInt(PIN / 100000) % 10);
    accum += 1 * (parseInt(PIN / 10000) % 10);
    accum += 3 * (parseInt(PIN / 1000) % 10);
    accum += 1 * (parseInt(PIN / 100) % 10);
    accum += 3 * (parseInt(PIN / 10) % 10);
    accum += 1 * (parseInt(PIN / 1) % 10);

    return ((accum % 10) == 0);
}

function PINPBCFormCheck()
{
	if (document.WPS.PINPBCRadio[0].checked) {
		// PIN
		if(document.WPS.PIN.value != "") {
			// Driver 1.9 supports 4 digit PIN code.
			if (document.WPS.PIN.value.length != 4 && !ValidateChecksum(document.WPS.PIN.value)) {
				alert("PIN number validation failed\n");
				return false;
			}
		}
	} else {
		// PBC
	}
	top.menu.setLockMenu(1);
	return true;
}

function checkSecurity()
{
	var authmode = '<% getCfgGeneral(1, "AuthMode"); %>';
	var ieee8021x = '<% getCfgGeneral(1, "IEEE8021X"); %>';
	var security = new Array(); 

	security = authmode.split(";");
	if (security[0] == "SHARED" || security[0] == "WEPAUTO" || security[0] == "WPA" || 
			security[0] == "WPA2" || security[0] == "WPA1WPA2" || ieee8021x == "1")
		alert("WPS does not support this security mode, please reset to OOB first!");
}

function initTranslation()
{
	var e = document.getElementById("wpsTitle_text");
	e.innerHTML = _("wps title");
	e = document.getElementById("wpsIntroduction_text");
	e.innerHTML = _("wps introduction");
	e = document.getElementById("wpsConfig_text");
	e.innerHTML = _("wps config");
	e = document.getElementById("wpsWPS_text");
	e.innerHTML = _("wps wps");
	e = document.getElementById("wpsDisable_text");
	e.innerHTML = _("wireless disable");
	e = document.getElementById("wpsEnable_text");
	e.innerHTML = _("wireless enable");
	e = document.getElementById("wpsConfigApply_text");
	e.value = _("wireless apply");

	e = document.getElementById("wpsSummary_text");
	e.innerHTML = _("wps summary");
	e = document.getElementById("wpsCurrentStatus_text");
	e.innerHTML = _("wps current status");
	e = document.getElementById("wpsConfigured_text");
	e.innerHTML = _("wps configured");
	e = document.getElementById("wpsSSID_text");
	e.innerHTML = _("wps ssid");
	e = document.getElementById("wpsAuthMode_text");
	e.innerHTML = _("wps auth mode");
	e = document.getElementById("wpsEncrypType_text");
	e.innerHTML = _("wps encryp type");
	e = document.getElementById("wpsDefaultKeyIndex_text");
	e.innerHTML = _("wps default key index");
	e = document.getElementById("wpsAPPIN_text");
	e.innerHTML = _("wps ap pin");
	e = document.getElementById("wpsResetOOB_text");
	e.value = _("wps reset oob");

	e = document.getElementById("wpsProgress_text");
	e.innerHTML = _("wps progress");
	e = document.getElementById("wpsMode_text");
	e.innerHTML = _("wps mode");
	e = document.getElementById("wpsPINMode_text");
	e.innerHTML = _("wps pin mode");
	e = document.getElementById("wpsPBCMode_text");
	e.innerHTML = _("wps pbc mode");
	e = document.getElementById("wpsPINNum_text");
	e.innerHTML = _("wps pin num");
	e = document.getElementById("wpsPINApply_text");
	e.value = _("wireless apply");
	
	e = document.getElementById("wpsStatus_text");
	e.innerHTML = _("wps status");

}

function pageInit()
{
	initTranslation();
	// hide tables first
	document.getElementById("div_wps_status").style.visibility = "hidden";
	document.getElementById("div_wps_status").style.display = "none";
	document.getElementById("div_wps").style.visibility = "hidden";
	document.getElementById("div_wps").style.display = "none";
	document.getElementById("div_wps_info").style.visibility = "hidden";
	document.getElementById("div_wps_info").style.display = "none";

	var wpsenable = "<% getCfgZero(1, "WscModeOption"); %>";
	if(wpsenable == "0"){
		// disable WPS
		document.getElementById("WPSEnable").options.selectedIndex = 0;
	}else{
		// enable WPS
		document.getElementById("WPSEnable").options.selectedIndex = 1;
		checkSecurity();

		document.getElementById("div_wps_status").style.visibility = "hidden";
		document.getElementById("div_wps_status").style.display = "none";
		document.getElementById("div_wps").style.visibility = "hidden";
		document.getElementById("div_wps").style.display = "none";
		document.getElementById("div_wps_info").style.visibility = "hidden";
		document.getElementById("div_wps_info").style.display = "none";

		document.getElementById("div_wps_status").style.visibility = "visible";
		document.getElementById("div_wps").style.visibility = "visible";
		document.getElementById("div_wps_info").style.visibility = "visible";


		// show WPS-related tables
		if (window.ActiveXObject) { // IE
			document.getElementById("div_wps_status").style.display = "block";
			document.getElementById("div_wps").style.display = "block";
			document.getElementById("div_wps_info").style.display = "block";
		}else if (window.XMLHttpRequest) { // Mozilla, Safari...
			document.getElementById("div_wps_status").style.display = "table";
			document.getElementById("div_wps").style.display = "table";
			document.getElementById("div_wps_info").style.display = "table";
 		}

		updateWPS();
		InitializeTimer(3);
	}
}


function onPINPBCRadioClick(value)
{
	if(value == 1){
		// PIN selected
		document.getElementById("PINRow").style.visibility = "visible";
		document.getElementById("PINRow").style.display = style_display_on();
	}else{
		// PBC selected
		document.getElementById("PINRow").style.visibility = "hidden";
		document.getElementById("PINRow").style.display = "none";

	}
}

function refresh_progress_bar(index)
{
	var bar_color = "blue";
	var clear = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
	var wps_progress;

	switch (1*index) {
		case 3:		// Start WSC Process
			wps_progress = 0;
			break;
		case 4:		// Received EAPOL-Start
			wps_progress = 1;
			break;
		case 5:		// Sending EAP-Req(ID)
		case 6:		// Receive EAP-Rsp(ID)
			wps_progress = 2;
			break;
		case 7:		// Receive EAP-Req with wrong WSC SMI Vendor Id
		case 8:		// Receive EAPReq with wrong WSC Vendor Type
		case 9:		// Sending EAP-Req(WSC_START)
			wps_progress = 3;
			break;
		case 10:	// Send M1
		case 11:	// Received M1
			wps_progress = 4;
			break;
		case 12:	// Send M2
		case 13:	// Received M2
			wps_progress = 5;
			break;
		case 15: 	// Send M3
		case 16:	// Received M3
			wps_progress = 6;
			break;
		case 17:	// Send M4
		case 18:	// Received M4
			wps_progress = 7;
			break;
		case 19:	// Send M5
		case 20:	// Received M5
			wps_progress = 8;
			break;
		case 21:	// Send M6
		case 22:	// Received M6
			wps_progress = 9;
			break;
		case 23:	// Send M7
		case 24:	// Received M7
			wps_progress = 10;
			break;
		case 25:	// Send M8
		case 26:	// Received M8
			wps_progress = 11;
			break;
		case 27:	// Processing EAP Response (ACK)
		case 28:	// Processing EAP Request (Done)
		case 29:	// Processing EAP Response (Done)
			wps_progress = 12;
			break;
		case 30:	// Sending EAP-Fail
		case 34:	// Configured
			wps_progress = 13;
			break;
	}
	for (i = 0; i <= wps_progress; i++) {
		var block = document.getElementById("block" + i);
		block.innerHTML = clear;
		block.style.backgroundColor = bar_color;
	}
}

function clear_progress_bar()
{
	index = 13;
	var bar_color = "white";
	var clear = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
		
	for (i = 0; i <= index; i++) {
		var block = document.getElementById("block" + i);
		block.innerHTML = clear;
		block.style.backgroundColor = bar_color;
	}
}

function WPSCancel()
{
	top.menu.setLockMenu(0);
	document.WPS.PINPBCRadio[0].disabled = false;
	document.WPS.PINPBCRadio[1].disabled = false;
	document.WPS.PIN.disabled = false;
	document.WPS.submitWPS.disabled = false;
	document.SubmitGenPIN.GenPIN.disabled = false;
	document.SubmitOOB.submitResetOOB.disabled = false;
	document.WPSConfig.WPSEnable.disabled = false;
	document.WPSConfig.submitWPSEnable.disabled = false;
	document.wps_cancel.submit();
	window.location.reload();
}
</script>

</head>
<body onload="pageInit()">
<table class="body"><tr><td>
<h1 id="wpsTitle_text">Wi-Fi Protected Setup</h1>
<p id="wpsIntroduction_text"> You could setup security easily by choosing PIN or PBC method to do Wi-Fi Protected Setup.</p>

<table border="1" cellpadding="2" cellspacing="1" width="90%">
<form method="post" name ="WPSConfig" action="/goform/WPSSetup">
<tbody>
<!-- ==================  WPS Config  ================== -->
<tr>
  <td class="title" colspan="2" id="wpsConfig_text">WPS Config</td>
</tr>

<tr>
  <td class="head" id="wpsWPS_text">WPS: </td>
  <td>	<select id="WPSEnable" name="WPSEnable" size="1">
			<option value=0 id="wpsDisable_text">Disable</option>
			<option value=1 id="wpsEnable_text">Enable</option>
		</select>
  </td>
</tr>

<tr>
<td colspan="2"> <input type="submit" value="Apply" id="wpsConfigApply_text" name="submitWPSEnable" align="left"> </td>
</tr>
</tbody>
</form>
</table>
<br />
<table id="div_wps_status" name="div_wps_status" border="1" cellpadding="2" cellspacing="1" width="90%" style="visibility: hidden;">
<tbody>

<!-- =================  WPS Summary  ================= -->
<tr>
  <td class="title" colspan="2" id="wpsSummary_text">WPS Summary</td>
</tr>

<tr>
  <td class="head" id="wpsCurrentStatus_text">WPS Current Status: </td>
  <td> <span id="WPSCurrentStatus"> </span> </td>
</tr>


<tr>
  <td class="head" id="wpsConfigured_text">WPS Configured: </td>
  <td> <span id="WPSConfigured"> </span> </td>
</tr>

<tr>
  <td class="head" id="wpsSSID_text">WPS SSID: </td>
  <td> <span id="WPSSSID"> </span> </td>
</tr>

<tr>
  <td class="head" id="wpsAuthMode_text">WPS Auth Mode: </td>
  <td> <span id="WPSAuthMode"> </span> </td>
</tr>

<tr>
  <td class="head" id="wpsEncrypType_text">WPS Encryp Type: </td>
  <td> <span id="WPSEncryptype"> </span> </td> 
</tr>

<tr>
  <td class="head" id="wpsDefaultKeyIndex_text">WPS Default Key Index: </td>
  <td> <span id="WPSDefaultKeyIndex"> </span> </td>
</tr>

<tr>
  <td class="head" > <span id="WPSKeyType"> </span></td>
  <td> <span id="WPSWPAKey"> </span> </td>
</tr>
<form method="post" name="SubmitGenPIN" action="/goform/GenPIN">
<tr>
  <td class="head" id="wpsAPPIN_text">AP PIN:
  </td>
  <td> <% getPINASP(); %> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type=submit value="Generate" name="GenPIN"></td>
</tr>
</form>
<tr>
  <td colspan="2">
	<form method="post" name ="SubmitOOB" action="/goform/OOB">
		<input type="submit" value="Reset OOB" id="wpsResetOOB_text" name="submitResetOOB" align="left">
	</form>
  <td>
</tr>
</tbody>
</table>
<br />
<!-- ============================    WPS    ============================ -->
<form method="post" name ="WPS" action="/goform/WPS">
<table id="div_wps" name="div_wps"  border="1" cellpadding="2" cellspacing="1" width="90%" style="visibility: hidden;">
<tbody>
<tr>
  <td class="title" colspan="2" id="wpsProgress_text">WPS Progress</td>
</tr>

<tr>
	<td class="head" id="wpsMode_text">WPS mode</td>
	<td>
		<input name="PINPBCRadio" id="PINPBCRadio" value="1" type="radio" checked onClick="onPINPBCRadioClick(1)"><font id="wpsPINMode_text">PIN &nbsp;</font>
		<input name="PINPBCRadio" id="PINPBCRadio" value="2" type="radio" onClick="onPINPBCRadioClick(2)"><font id="wpsPBCMode_text">PBC &nbsp;</font>
	</td>
</tr>

<tr id="PINRow">
	<td class="head" id="wpsPINNum_text">PIN</td>
	<td>
		<input value="" name="PIN" id="PIN" size="8" maxlength="16" type="text">
	</td>
</tr>

<tr>
	<td colspan="2">
		<input type="submit" value="Apply" id="wpsPINApply_text" name="submitWPS" align="left" onClick="return PINPBCFormCheck();">
	</td>
</tr>
</tbody>
</table>
</form>
<br />
<!-- =======================  WPS Info Bar  ======================= -->
<table id="div_wps_info" name="div_wps_info" border="1" cellpadding="1" cellspacing="1" width="90%" style="visibility: hidden;">
<tbody><tr><td colspan="2" class="title" id="wpsStatus_text">WPS Status</td></tr>
<tr><td colspan="2"> 
<textarea name="WPSInfo" id="WPSInfo" cols="55" rows="2" wrap="off" readonly="1"></textarea>
</td></tr>
<tr>
  <td>
    <div id="wps_progress_bar">
      <span id="block0">&nbsp;</span>
      <span id="block1">&nbsp;</span>
      <span id="block2">&nbsp;</span>
      <span id="block3">&nbsp;</span>
      <span id="block4">&nbsp;</span>
      <span id="block5">&nbsp;</span>
      <span id="block6">&nbsp;</span>
      <span id="block7">&nbsp;</span>
      <span id="block8">&nbsp;</span>
      <span id="block9">&nbsp;</span>
      <span id="block10">&nbsp;</span>
      <span id="block11">&nbsp;</span>
      <span id="block12">&nbsp;</span>
      <span id="block13">&nbsp;</span>
    </div>
  </td>
  <form method="post" name="wps_cancel" action="/goform/WPSCancel">
    <td align="center"><input type="button" value="Cancel" id="wpsCancel" name="wpsCancel" onClick="WPSCancel()"></td>
  </form>
</tr>
</tbody></table>
</td></tr></table>
</body></html>
