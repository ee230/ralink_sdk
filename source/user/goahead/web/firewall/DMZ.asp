<html>
<head>
<title>DMZ Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("firewall");

var superdmzbuilt = "<% getSuperDMZBuilt(); %>";

var dmz_enable = "<% getCfgGeneral(1, "DMZEnable"); %>";
var dmz_address = "<% getCfgGeneral(1, "DMZAddress"); %>";
var dmz_tcpport80 = "<% getCfgGeneral(1, "DMZAvoidTCPPort80"); %>";

function deleteClick()
{
    return true;
}

function checkRange(str, num, min, max)
{
	d = atoi(str,num);
	if(d > max || d < min)
		return false;
	return true;
}

function atoi(str, num)
{
	i=1;
	if(num != 1 ){
		while (i != num && str.length != 0){
			if(str.charAt(0) == '.'){
				i++;
			}
			str = str.substring(1);
		}
	  	if(i != num )
			return -1;
	}
	
	for(i=0; i<str.length; i++){
		if(str.charAt(i) == '.'){
			str = str.substring(0, i);
			break;
		}
	}
	if(str.length == 0)
		return -1;
	return parseInt(str, 10);
}

function checkMac(str){
    var len = str.length;
    if(len!=17)
        return false;

    for (var i=0; i<str.length; i++) {
        if((i%3) == 2){
            if(str.charAt(i) == ':')
                continue;
        }else{
            if (    (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
                    (str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
                    (str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
                continue;
        }
        return false;
    }
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

function checkIPAddr(field)
{
	if(field.value == ""){
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if ( isAllNum(field.value) == 0) {
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if( (!checkRange(field.value,1,0,255)) ||
		(!checkRange(field.value,2,0,255)) ||
		(!checkRange(field.value,3,0,255)) ||
		(!checkRange(field.value,4,1,254)) ){
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	return true;
}

function formCheck()
{
	if(!document.DMZ.DMZEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if(document.DMZ.DMZAddress.value == ""){
		alert("Please assign an address.");
		document.DMZ.DMZAddress.focus();
		return false;
	}

	if(document.DMZ.DMZEnabled.options.selectedIndex == 1){
		if(! checkIPAddr(document.DMZ.DMZAddress) ){
			alert("IP address format error.");
			document.DMZ.DMZAddress.focus();
			return false;
		}
	}else if(document.DMZ.DMZEnabled.options.selectedIndex == 2){
		if(! checkMac(document.DMZ.DMZAddress.value) ){
			alert("MAC address format error.");
			document.DMZ.DMZAddress.focus();
			return false;
		}
	}

	return true;
}


function display_on()
{
  if(window.XMLHttpRequest){ // Mozilla, Firefox, Safari,...
    return "table-row";
  } else if(window.ActiveXObject){ // IE
    return "block";
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

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function initTranslation()
{
	var e = document.getElementById("dmzTitle");
	e.innerHTML = _("dmz title");
	e = document.getElementById("dmzIntroduction");
	e.innerHTML = _("dmz introduction");

	e = document.getElementById("dmzSetting");
	e.innerHTML = _("dmz setting");
	e = document.getElementById("dmzSet");
	e.innerHTML = _("dmz setting");
	e = document.getElementById("dmzDisable");
	e.innerHTML = _("firewall disable");
	e = document.getElementById("dmzEnable");
	e.innerHTML = _("firewall enable");
	e = document.getElementById("superdmzEnable");
	e.innerHTML = _("firewall enable") + "SuperDMZ";
	e = document.getElementById("dmzTCPPort80Str");
	e.innerHTML = e.innerHTML+_("dmz tcpport80");
	e = document.getElementById("dmzApply");
	e.value = _("firewall apply");
	e = document.getElementById("dmzReset");
	e.value = _("firewall reset");
}

function init()
{
	initTranslation();
	document.DMZ.DMZTCPPort80.checked = false;
	document.DMZ.DMZTCPPort80.value = "0";
	if(dmz_enable == "0"){
		document.DMZ.DMZEnabled.options.selectedIndex = 0;
		document.DMZ.DMZTCPPort80.disabled = true;
		disableTextField(document.DMZ.DMZAddress);
	}else{
		enableTextField(document.DMZ.DMZAddress);
		document.DMZ.DMZTCPPort80.disabled = false;
		document.DMZ.DMZAddress.value = dmz_address;
		if(dmz_enable == "1"){
			document.DMZ.DMZEnabled.options.selectedIndex = 1;
			e = document.getElementById("dmzAddr");
			e.innerHTML = _("dmz ipaddr");
		}else if(dmz_enable == "2"){
			document.DMZ.DMZEnabled.options.selectedIndex = 2;
			e = document.getElementById("dmzAddr");
			e.innerHTML = _("dmz macaddr");
		}
		if(dmz_tcpport80 != "" &&dmz_tcpport80 != "0"){
			document.DMZ.DMZTCPPort80.value = "1";
			document.DMZ.DMZTCPPort80.checked = true;
		}
	}

	if(superdmzbuilt != "1")
		document.DMZ.DMZEnabled.options[2] = null;
}

function selectChange()
{
	document.DMZ.DMZAddress.value = "";
	if(document.DMZ.DMZEnabled.options.selectedIndex == 0){
		disableTextField(document.DMZ.DMZAddress);
		document.DMZ.DMZTCPPort80.disabled = true;
	}else{
		enableTextField(document.DMZ.DMZAddress);
		document.DMZ.DMZTCPPort80.disabled = false;
		if(document.DMZ.DMZEnabled.options.selectedIndex == 1){
			e = document.getElementById("dmzAddr");
			e.innerHTML = _("dmz ipaddr");
		}else if(document.DMZ.DMZEnabled.options.selectedIndex == 2){
			e = document.getElementById("dmzAddr");
			e.innerHTML = _("dmz macaddr");
		}
	}
}

function dmzClick()
{
	if(!formCheck())
		return false;
	document.DMZ.DMZTCPPort80.value = document.DMZ.DMZTCPPort80.checked ? "1": "0";
	return true;
}

</script>
</head>


<!--     body      -->
<body onload="init()">
<table class="body"><tr><td>
<h1 id="dmzTitle"> DMZ Settings </h1>
<p id="dmzIntroduction"> You may setup a De-militarized Zone(DMZ) to separate internal network and Internet.</p>
<hr />

<form method=post name="DMZ" action=/goform/DMZ>
<table width="400" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="dmzSetting">DMZ Settings</td>
</tr>
<tr>
	<td class="head" id="dmzSet">
		DMZ Settings
	</td>
	<td>
	<select onChange="selectChange()" name="DMZEnabled" size="1">
	<option value=0 id="dmzDisable">Disable DMZ</option>
    <option value=1 id="dmzEnable">Enable DMZ</option>
    <option value=2 id="superdmzEnable">Enable SuperDMZ</option>
    </select>
    </td>
</tr>

<tr>
	<td class="head" id="dmzAddr">
		DMZ Address
	</td>
	<td>
  		<input type="text" size="24" name="DMZAddress" >
	</td>
</tr>
</table>
<table width="95%" cellpadding="2" cellspacing="1">
<tr align="right"><td id=dmzTCPPort80Str>
<input type=checkbox name="DMZTCPPort80" id="DMZTCPPort80" value="0"></td></tr>
</table>
<p>
	<input type="submit" value="Apply" id="dmzApply" name="addDMZ" onClick="return dmzClick()"> &nbsp;&nbsp;
	<input type="reset" value="Reset" id="dmzReset" name="reset">
</p>
</form>

<br>
<hr />

</td></tr></table>
</body>
</html>
