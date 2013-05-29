<html><head><title>QoS/Bandwidth Management Settings</title>

<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("internet");

var QOS_MAX_HARD_LIMITED = "100M";						// Our max bandwidth that we can deal with.
var QOS_MIN_HARD_LIMITED = "32k";						// Our min bandwidth that we hope.

var QoS = "<% getCfgGeneral(1, "QoSEnable"); %>";
var QoSModel = "<% getCfgGeneral(1, "QoSModel"); %>";
var uploadBandwidth = "<% getCfgGeneral(1, "QoSUploadBandwidth"); %>";
var downloadBandwidth = "<% getCfgGeneral(1, "QoSDownloadBandwidth"); %>";
var uploadBandwidth_custom = "<% getCfgGeneral(1, "QoSUploadBandwidth_custom"); %>";
var downloadBandwidth_custom = "<% getCfgGeneral(1, "QoSDownloadBandwidth_custom"); %>";
var reserveBandwidth = "<% getCfgGeneral(1, "QoSReserveBandwidth"); %>";
var isPortBasedQoSSupport = "<% QoSisPortBasedQoSSupport(); %>";


QoS = (QoS == "") ? "0" : QoS;
QoSModel = (QoSModel == "") ? "1" : QoSModel;		/* Default model is DRR */
reserveBandwidth = (reserveBandwidth == "") ? "0" : reserveBandwidth;

/*
	Upload AF Group init
*/
var AF1ULName = "<% getCfgGeneral(1, "QoSAF1ULName"); %>";
var AF2ULName = "<% getCfgGeneral(1, "QoSAF2ULName"); %>";
var AF3ULName = "<% getCfgGeneral(1, "QoSAF3ULName"); %>";
var AF4ULName = "<% getCfgGeneral(1, "QoSAF4ULName"); %>";
var AF5ULName = "<% getCfgGeneral(1, "QoSAF5ULName"); %>";
var AF6ULName = "<% getCfgGeneral(1, "QoSAF6ULName"); %>";
AF1ULName = (AF1ULName == "") ? _("qos Low") : AF1ULName;
AF2ULName = (AF2ULName == "") ? _("qos High") : AF2ULName;
AF3ULName = (AF3ULName == "") ? "NoName3(UL)" : AF3ULName;
AF4ULName = (AF4ULName == "") ? "NoName4(UL)" : AF4ULName;
AF5ULName = (AF5ULName == "") ? _("qos Highest") : AF5ULName;
AF6ULName = (AF6ULName == "") ? _("qos Default") : AF6ULName;

var AF1ULRate = "<% getCfgGeneral(1, "QoSAF1ULRate"); %>";
var AF2ULRate = "<% getCfgGeneral(1, "QoSAF2ULRate"); %>";
var AF3ULRate = "<% getCfgGeneral(1, "QoSAF3ULRate"); %>";
var AF4ULRate = "<% getCfgGeneral(1, "QoSAF4ULRate"); %>";
var AF5ULRate = "<% getCfgGeneral(1, "QoSAF5ULRate"); %>";
var AF6ULRate = "<% getCfgGeneral(1, "QoSAF6ULRate"); %>";
AF1ULRate = (AF1ULRate == "") ? "10" : AF1ULRate;		// default 10% 
AF2ULRate = (AF2ULRate == "") ? "10" : AF2ULRate;
AF3ULRate = (AF3ULRate == "") ? "10" : AF3ULRate;
AF4ULRate = (AF4ULRate == "") ? "10" : AF4ULRate;
AF5ULRate = (AF5ULRate == "") ? "10" : AF5ULRate;
AF6ULRate = (AF6ULRate == "") ? "10" : AF6ULRate;

var AF1ULCeil = "<% getCfgGeneral(1, "QoSAF1ULCeil"); %>";
var AF2ULCeil = "<% getCfgGeneral(1, "QoSAF2ULCeil"); %>";
var AF3ULCeil = "<% getCfgGeneral(1, "QoSAF3ULCeil"); %>";
var AF4ULCeil = "<% getCfgGeneral(1, "QoSAF4ULCeil"); %>";
var AF5ULCeil = "<% getCfgGeneral(1, "QoSAF5ULCeil"); %>";
var AF6ULCeil = "<% getCfgGeneral(1, "QoSAF6ULCeil"); %>";
AF1ULCeil = (AF1ULCeil == "") ? "100" : AF1ULCeil;		// default 100%
AF2ULCeil = (AF2ULCeil == "") ? "100" : AF2ULCeil;
AF3ULCeil = (AF3ULCeil == "") ? "100" : AF3ULCeil;
AF4ULCeil = (AF4ULCeil == "") ? "100" : AF4ULCeil;
AF5ULCeil = (AF5ULCeil == "") ? "100" : AF5ULCeil;
AF6ULCeil = (AF6ULCeil == "") ? "100" : AF6ULCeil;


/*
	Download AF Group init
*/
var AF1DLName = "<% getCfgGeneral(1, "QoSAF1DLName"); %>";
var AF2DLName = "<% getCfgGeneral(1, "QoSAF2DLName"); %>";
var AF3DLName = "<% getCfgGeneral(1, "QoSAF3DLName"); %>";
var AF4DLName = "<% getCfgGeneral(1, "QoSAF4DLName"); %>";
var AF5DLName = "<% getCfgGeneral(1, "QoSAF5DLName"); %>";
var AF6DLName = "<% getCfgGeneral(1, "QoSAF6DLName"); %>";
AF1DLName = (AF1DLName == "") ? _("qos Low") : AF1DLName;
AF2DLName = (AF2DLName == "") ? _("qos High") : AF2DLName;
AF3DLName = (AF3DLName == "") ? "NoName3(DL)" : AF3DLName;
AF4DLName = (AF4DLName == "") ? "NoName4(DL)" : AF4DLName;
AF5DLName = (AF5DLName == "") ? _("qos Highest") : AF5DLName;
AF6DLName = (AF6DLName == "") ? _("qos Default") : AF6DLName;

var AF1DLRate = "<% getCfgGeneral(1, "QoSAF1DLRate"); %>";
var AF2DLRate = "<% getCfgGeneral(1, "QoSAF2DLRate"); %>";
var AF3DLRate = "<% getCfgGeneral(1, "QoSAF3DLRate"); %>";
var AF4DLRate = "<% getCfgGeneral(1, "QoSAF4DLRate"); %>";
var AF5DLRate = "<% getCfgGeneral(1, "QoSAF5DLRate"); %>";
var AF6DLRate = "<% getCfgGeneral(1, "QoSAF6DLRate"); %>";
AF1DLRate = (AF1DLRate == "") ? "10" : AF1DLRate;		// default 10% 
AF2DLRate = (AF2DLRate == "") ? "10" : AF2DLRate;
AF3DLRate = (AF3DLRate == "") ? "10" : AF3DLRate;
AF4DLRate = (AF4DLRate == "") ? "10" : AF4DLRate;
AF5DLRate = (AF5DLRate == "") ? "10" : AF5DLRate;
AF6DLRate = (AF6DLRate == "") ? "10" : AF6DLRate;

var AF1DLCeil = "<% getCfgGeneral(1, "QoSAF1DLCeil"); %>";
var AF2DLCeil = "<% getCfgGeneral(1, "QoSAF2DLCeil"); %>";
var AF3DLCeil = "<% getCfgGeneral(1, "QoSAF3DLCeil"); %>";
var AF4DLCeil = "<% getCfgGeneral(1, "QoSAF4DLCeil"); %>";
var AF5DLCeil = "<% getCfgGeneral(1, "QoSAF5DLCeil"); %>";
var AF6DLCeil = "<% getCfgGeneral(1, "QoSAF6DLCeil"); %>";
AF1DLCeil = (AF1DLCeil == "") ? "100" : AF1DLCeil;		// default 100%
AF2DLCeil = (AF2DLCeil == "") ? "100" : AF2DLCeil;
AF3DLCeil = (AF3DLCeil == "") ? "100" : AF3DLCeil;
AF4DLCeil = (AF4DLCeil == "") ? "100" : AF4DLCeil;
AF5DLCeil = (AF5DLCeil == "") ? "100" : AF5DLCeil;
AF6DLCeil = (AF6DLCeil == "") ? "100" : AF6DLCeil;

var ULRules = "<% getCfgGeneral(1, "QoSULRules"); %>";
var DLRules = "<% getCfgGeneral(1, "QoSDLRules"); %>";

var port_based_settings = "<% getCfgGeneral(1, "QoSPortBasedRules"); %>";
port_based_settings = port_based_settings.length ? port_based_settings : ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";

var port0 = "2";		/* "0" : highest, "1": high, "2": default, "3": low */
var port1 = "2";
var port2 = "2";
var port3 = "2";
var port4 = "2";
var port0_remark = "0";
var port1_remark = "0";
var port2_remark = "0";
var port3_remark = "0";
var port4_remark = "0";

/*
var ssid1 = "<% getCfgGeneral(1, "SSID1"); %>";
var ssid2 = "<% getCfgGeneral(1, "SSID2"); %>";
var ssid3 = "<% getCfgGeneral(1, "SSID3"); %>";
var ssid4 = "<% getCfgGeneral(1, "SSID4"); %>";
var ssid5 = "<% getCfgGeneral(1, "SSID5"); %>";
var ssid6 = "<% getCfgGeneral(1, "SSID6"); %>";
var ssid7 = "<% getCfgGeneral(1, "SSID7"); %>";
var ssid8 = "<% getCfgGeneral(1, "SSID8"); %>";
*/
var ssid1 = "";
var ssid2 = "";
var ssid3 = "";
var ssid4 = "";
var ssid5 = "";
var ssid6 = "";
var ssid7 = "";
var ssid8 = "";

var ssid1_group = "2";
var ssid2_group = "2";
var ssid3_group = "2";
var ssid4_group = "2";
var ssid5_group = "2";
var ssid6_group = "2";
var ssid7_group = "2";
var ssid8_group = "2";
var ssid1_remark = "0";
var ssid2_remark = "0";
var ssid3_remark = "0";
var ssid4_remark = "0";
var ssid5_remark = "0";
var ssid6_remark = "0";
var ssid7_remark = "0";
var ssid8_remark = "0";

function getGroupNameByIndex(index)
{
	if(index == "5")
		return _("qos Highest");
	if(index == "2")
			return _("qos High");
	if(index == "6")
			return _("qos Default");
	if(index == "1")
			return _("qos Low");
	return "Unknown";
}

function modifyAF(index, dir)
{
	var tmp = eval('AF'+ index+ dir + 'Name');
	document.trans.ModifyAFIndex.value = "" + index;
	if(tmp.length){
		if(dir == 'DL')
			document.trans.ModifyAFDir.value = "Download";
		else
			document.trans.ModifyAFDir.value = "Upload";
		document.trans.ModifyAFName.value = eval('AF'+index+ dir + 'Name');
		document.trans.ModifyAFRate.value = eval('AF'+index+ dir + 'Rate');
		document.trans.ModifyAFCeil.value = eval('AF'+index+ dir + 'Ceil');
	}else{
		document.trans.ModifyAFDir.value = "Unknown";
		document.trans.ModifyAFName.value = "";
		document.trans.ModifyAFRate.value = "";
		document.trans.ModifyAFCeil.value = "";
	}
	openwindow = window.open("qosaf.asp", "AF"+index, "toolbar=no, location=yes, scrollbars=no, resizable=no, width=600, height=200");
	openwindow.focus();
}


function AddRule(dir)
{
	document.trans_addrule.AddruleDir.value = dir;
	openwindow = window.open("qosclassifier.asp", "Add", "toolbar=no, location=yes, scrollbars=no, resizable=yes, width=600, height=700");
	openwindow.focus();
}


function initTranslation()
{
	var e;

	e = document.getElementById("QoSTitleStr");
	e.innerHTML = _("qos title");
	e = document.getElementById("QoSIntroStr");
	e.innerHTML = _("qos intro");
	e = document.getElementById("QoSSetupStr");
	e.innerHTML = _("qos setup");
	e = document.getElementById("QoSModelStr");
	e.innerHTML = _("qos model");
	e = document.getElementById("ReserveBandwidthStr");
	e.innerHTML = _("qos reservebandwidth");
	e = document.getElementById("QoSUploadTitleStr");
	e.innerHTML = _("qos upload title");
	e = document.getElementById("QoSDownloadTitleStr");
	e.innerHTML = _("qos download title");
	e = document.getElementById("QoSStr");
	e.innerHTML = _("qos str");
	e = document.getElementById("QoSDisableStr");
	e.innerHTML = _("qos disable");
	e = document.getElementById("QoSEnableBidirectionStr");
	e.innerHTML = _("qos enable bidirection");
	e = document.getElementById("QoSEnableUploadOnlyStr");
	e.innerHTML = _("qos enable upload only");
	e = document.getElementById("QoSEnableDownloadOnlyStr");
	e.innerHTML = _("qos enable download only");
/*	e = document.getElementById("QoSDSCPMakerOnlyStr");
	e.innerHTML = _("qos dscpmarkeronly");
*/	e = document.getElementById("QoSUpBWStr");
	e.innerHTML = _("qos upload bandwidth");
	e = document.getElementById("QoSBWCustomStr");
	e.innerHTML = _("qos bandwidth custom");
	e = document.getElementById("QoSBWCustomStr2");
	e.innerHTML = _("qos bandwidth custom");
	e = document.getElementById("QoSDownBWStr");
	e.innerHTML = _("qos download bandwidth");

	e = document.getElementById("QoSDownloadHighestStr");
	e.innerHTML = _("qos Highest");
	e = document.getElementById("QoSDownloadHighStr");
	e.innerHTML = _("qos High");
	e = document.getElementById("QoSDownloadDefaultStr");
	e.innerHTML = _("qos Default");
	e = document.getElementById("QoSDownloadLowStr");
	e.innerHTML = _("qos Low");
	e = document.getElementById("QoSUploadHighestStr");
	e.innerHTML = _("qos Highest");
	e = document.getElementById("QoSUploadHighStr");
	e.innerHTML = _("qos High");
	e = document.getElementById("QoSUploadDefaultStr");
	e.innerHTML = _("qos Default");
	e = document.getElementById("QoSUploadLowStr");
	e.innerHTML = _("qos Low");

	e = document.getElementById("QoSSetupSubmitStr");
	e.value = _("qos setup submit");

	e = document.getElementById("QoSRuleNoStr");
	e.innerHTML = _("qos rule no");
	e = document.getElementById("QoSRuleNameStr");
	e.innerHTML = _("qos rule name");
	e = document.getElementById("QoSRulePrioStr");
	e.innerHTML = _("qos rule prio");
	e = document.getElementById("QoSRuleInfoStr");
	e.innerHTML = _("qos rule info");
	e = document.getElementById("QoSRuleNoStr2");
	e.innerHTML = _("qos rule no");
	e = document.getElementById("QoSRuleNameStr2");
	e.innerHTML = _("qos rule name");
	e = document.getElementById("QoSRulePrioStr2");
	e.innerHTML = _("qos rule prio");
	e = document.getElementById("QoSRuleInfoStr2");
	e.innerHTML = _("qos rule info");

	e = document.getElementById("QoSRuleAddStr");
	e.value = _("qos rule add");
	e = document.getElementById("QoSRuleDelStr");
	e.value = _("qos rule del");
	e = document.getElementById("QoSDLRuleAddStr");
	e.value = _("qos rule dl add");
	e = document.getElementById("QoSDLRuleDelStr");
	e.value = _("qos rule dl del");

//	e = document.getElementById("QoSLoadProfileStr");
//	e.value = _("qos loaddefault");
//	e = document.getElementById("QoSRate");
//	e.value = _("qos Rate");

	/*
		_("qos Rate")
		_("qos Ceil")
	*/

	e = document.getElementById("QoSPortSetupStr");
	e.innerHTML = _("qos port setup");

	e = document.getElementById("QoSPortSetupSubmitStr");
	e.value = _("qos setup submit");

	e = document.getElementById("QoSPort0Str");
	e.innerHTML = _("qos port0");
	e = document.getElementById("QoSPort1Str");
	e.innerHTML = _("qos port1");
	e = document.getElementById("QoSPort2Str");
	e.innerHTML = _("qos port2");
	e = document.getElementById("QoSPort3Str");
	e.innerHTML = _("qos port3");
	e = document.getElementById("QoSPort4Str");
	e.innerHTML = _("qos port4");

}

function hideULRulesTable()
{
	document.getElementById("div_qos_ul_rules").style.visibility = "hidden";
	document.getElementById("div_qos_ul_rules").style.display = "none";
}
function hideDLRulesTable()
{
	document.getElementById("div_qos_dl_rules").style.visibility = "hidden";
	document.getElementById("div_qos_dl_rules").style.display = "none";
}

function showULRulesTable()
{
	if (window.ActiveXObject) { // IE
		document.getElementById("div_qos_ul_rules").style.display = "block";
	}else if (window.XMLHttpRequest) { // Mozilla, Safari...
		document.getElementById("div_qos_ul_rules").style.display = "table";
	}
}

function showDLRulesTable()
{
	if (window.ActiveXObject) { // IE
		document.getElementById("div_qos_dl_rules").style.display = "block";
	}else if (window.XMLHttpRequest) { // Mozilla, Safari...
		document.getElementById("div_qos_dl_rules").style.display = "table";
	}
}

function showULRule()
{
	if (window.ActiveXObject) { // IE
		document.getElementById("div_qos_ul_group_test").style.display = "block";
		document.getElementById("div_qos_ul_rules").style.display = "block";
		document.getElementById("div_qos_loaddefault").style.display = "block";

	}else if (window.XMLHttpRequest) { // Mozilla, Safari...
		document.getElementById("div_qos_ul_group_test").style.display = "table";
		document.getElementById("div_qos_ul_rules").style.display = "table";
		document.getElementById("div_qos_loaddefault").style.display = "table";
	}

	document.getElementById("div_qos_ul_group_test").style.visibility = "visible";
	document.getElementById("div_qos_ul_rules").style.visibility = "visible";
	document.getElementById("div_qos_loaddefault").style.visibility = "visible";

	document.QoSDeleteULRules.add_ul_rule.style.display = '';
	document.QoSDeleteULRules.del_ul_rule.style.display = '';
}

function showDLRule()
{
	if (window.ActiveXObject) { // IE
		document.getElementById("div_qos_dl_group_test").style.display = "block";
		document.getElementById("div_qos_dl_rules").style.display = "block";
		document.getElementById("div_qos_loaddefault").style.display = "block";

	}else if (window.XMLHttpRequest) { // Mozilla, Safari...
		document.getElementById("div_qos_dl_group_test").style.display = "table";
		document.getElementById("div_qos_dl_rules").style.display = "table";
		document.getElementById("div_qos_loaddefault").style.display = "table";
	}

	document.getElementById("div_qos_dl_group_test").style.visibility = "visible";
	document.getElementById("div_qos_dl_rules").style.visibility = "visible";
	document.getElementById("div_qos_loaddefault").style.visibility = "visible";

	document.QoSDeleteDLRules.add_dl_rule.style.display = '';
	document.QoSDeleteDLRules.del_dl_rule.style.display = '';
}

function hideULRule()
{
	document.QoSDeleteULRules.add_ul_rule.style.display = 'none';
	document.QoSDeleteULRules.del_ul_rule.style.display = 'none';

	document.getElementById("div_qos_ul_group_test").style.visibility = "hidden";
	document.getElementById("div_qos_ul_group_test").style.display = "none";

	hideULRulesTable();
}

function hideDLRule()
{
	document.QoSDeleteDLRules.add_dl_rule.style.display = 'none';
	document.QoSDeleteDLRules.del_dl_rule.style.display = 'none';

	document.getElementById("div_qos_dl_group_test").style.visibility = "hidden";
	document.getElementById("div_qos_dl_group_test").style.display = "none";

	hideDLRulesTable();
}

function onInit()
{
	initTranslation();

	document.QoSSetup.UploadBandwidth_Custom.disabled = true;
	document.QoSSetup.DownloadBandwidth_Custom.disabled = true;
	document.QoSSetup.Model.disabled = true;
	document.QoSSetup.UploadBandwidth_Custom.style.display = 'none';
	document.QoSSetup.DownloadBandwidth_Custom.style.display = 'none';

	hideULRule();
	hideDLRule();

	UploadQueueHide();
	DownloadQueueHide();

	if(QoS == "0"){
		document.QoSSetup.UploadBandwidth.disabled = true;
		document.QoSSetup.DownloadBandwidth.disabled = true;
		document.QoSSetup.Model.disabled = true;
		document.QoSSetup.ReserveBandwidth.disabled = true;
	}else{
		document.QoSSetup.UploadBandwidth.disabled = false;
		document.QoSSetup.DownloadBandwidth.disabled = false;
		document.QoSSetup.Model.disabled = false;
		document.QoSSetup.ReserveBandwidth.disabled = false;

		if(QoS == "1"){
			//  Both ingress & egress enabled 
			UploadQueueShow();
			DownloadQueueShow();

			showULRule();
			showDLRule();
		}else if(QoS == "2"){
			//   Egress enabled only
			UploadQueueShow();
			showULRule();
		}else if(QoS == "3"){
			//  Ingress enabled only
			DownloadQueueShow();
			showDLRule();

		}else if(QoS == "4" /* port based */){
			UploadQueueShow();
		}

		if(uploadBandwidth == "custom"){
			document.QoSSetup.UploadBandwidth_Custom.disabled = false;
			document.QoSSetup.UploadBandwidth_Custom.style.display = '';
			document.QoSSetup.UploadBandwidth_Custom.value = uploadBandwidth_custom;
		}else{
			for(var i=0; i < document.QoSSetup.UploadBandwidth.length; i++){
				if(document.QoSSetup.UploadBandwidth.options[i].value == uploadBandwidth){
					document.QoSSetup.UploadBandwidth.options.selectedIndex = i;
					break;
				}
			}
		}
		if(downloadBandwidth == "custom"){
			document.QoSSetup.DownloadBandwidth_Custom.disabled = false;
			document.QoSSetup.DownloadBandwidth_Custom.style.display = '';
			document.QoSSetup.DownloadBandwidth_Custom.value = downloadBandwidth_custom;
		}else{
			for(var i=0; i< document.QoSSetup.DownloadBandwidth.length; i++){
				if(document.QoSSetup.DownloadBandwidth.options[i].value == downloadBandwidth){
					document.QoSSetup.DownloadBandwidth.options.selectedIndex = i;
					break;
				}
			}
		}
		if(QoSModel == 1 /* DRR */)
			document.QoSSetup.Model.options.selectedIndex = 0;
		else if(QoSModel == 2 /* SPQ */)
			document.QoSSetup.Model.options.selectedIndex = 1;
		else if(QoSModel == 3 /* DRR+SPQ */)
			document.QoSSetup.Model.options.selectedIndex = 2;
		else if(QoSModel == 4 /* Remark only */)
			document.QoSSetup.Model.options.selectedIndex = 3;
		else
			document.QoSSetup.Model.options.selectedIndex = 0;

		/* Init Reserve Bandwidth */
		if( reserveBandwidth == "0")
			document.QoSSetup.ReserveBandwidth.options.selectedIndex = 0;
		else if( reserveBandwidth == "10")
			document.QoSSetup.ReserveBandwidth.options.selectedIndex = 1;
		else if( reserveBandwidth == "20")
			document.QoSSetup.ReserveBandwidth.options.selectedIndex = 2;
		else if( reserveBandwidth == "30")
			document.QoSSetup.ReserveBandwidth.options.selectedIndex = 3;
		else if( reserveBandwidth == "40")
			document.QoSSetup.ReserveBandwidth.options.selectedIndex = 4;
	}

	if(QoS == "0")
		document.QoSSetup.QoSSelect.options.selectedIndex = 0;
	else if(QoS == "1")
		document.QoSSetup.QoSSelect.options.selectedIndex = 1;
	else if(QoS == "2")
		document.QoSSetup.QoSSelect.options.selectedIndex = 2;
	else if(QoS == "3")
		document.QoSSetup.QoSSelect.options.selectedIndex = 3;
	else if(QoS == "4")
		document.QoSSetup.QoSSelect.options.selectedIndex = 4;

	QoSSetupQueue(document.QoSSetup.highest_upload_queue_rate, AF5ULRate);
	QoSSetupQueue(document.QoSSetup.highest_upload_queue_ceil, AF5ULCeil);
	QoSSetupQueue(document.QoSSetup.high_upload_queue_rate   , AF2ULRate);
	QoSSetupQueue(document.QoSSetup.high_upload_queue_ceil   , AF2ULCeil);
	QoSSetupQueue(document.QoSSetup.default_upload_queue_rate, AF6ULRate);
	QoSSetupQueue(document.QoSSetup.default_upload_queue_ceil, AF6ULCeil);
	QoSSetupQueue(document.QoSSetup.low_upload_queue_rate    , AF1ULRate);
	QoSSetupQueue(document.QoSSetup.low_upload_queue_ceil    , AF1ULCeil);

	QoSSetupQueue(document.QoSSetup.highest_download_queue_rate, AF5DLRate);
	QoSSetupQueue(document.QoSSetup.highest_download_queue_ceil, AF5DLCeil);
	QoSSetupQueue(document.QoSSetup.high_download_queue_rate   , AF2DLRate);
	QoSSetupQueue(document.QoSSetup.high_download_queue_ceil   , AF2DLCeil);
	QoSSetupQueue(document.QoSSetup.default_download_queue_rate, AF6DLRate);
	QoSSetupQueue(document.QoSSetup.default_download_queue_ceil, AF6DLCeil);
	QoSSetupQueue(document.QoSSetup.low_download_queue_rate    , AF1DLRate);
	QoSSetupQueue(document.QoSSetup.low_download_queue_ceil    , AF1DLCeil);

	if(QoSModel == 1 /* DRR */ ){
		QoSShowAllRateCeilSelect();
	}else if(QoSModel == 2 /* SPQ */ ){
		QoSHideAllRateCeilSelect();
	}else if(QoSModel == 3 /* SPQ+DRR*/ ){
		div_hide("highest_upload_queue_div");
		div_hide("high_upload_queue_div");
		div_show("default_upload_queue_div");
		div_show("low_upload_queue_div");
		div_hide("highest_download_queue_div");
		div_hide("high_download_queue_div");
		div_show("default_download_queue_div");
		div_show("low_download_queue_div");
	}else if(QoSModel == 4 /* Remark Only */ ){
		UploadQueueHide();
		DownloadQueueHide();
	}

	// port based QoS init
	if(QoS == "4"){
		div_show("QoSPortBasedDiv");

		/*
		 * Port based init
		 */ 	 
		var all_str = new Array();
		all_str = port_based_settings.split(",");
		/* Ethernet */
		port0 = all_str[0].length ? all_str[0] : port0;
		port0_remark = all_str[1].length ? all_str[1] : port0_remark;
		port1 = all_str[2].length ? all_str[2] : port1;
		port1_remark = all_str[3].length ? all_str[3] : port1_remark;
		port2 = all_str[4].length ? all_str[4] : port2;
		port2_remark = all_str[5].length ? all_str[5] : port2_remark;
		port3 = all_str[6].length ? all_str[6] : port3;
		port3_remark = all_str[7].length ? all_str[7] : port3_remark;
		port4 = all_str[8].length ? all_str[8] : port4;
		port4_remark = all_str[9].length ? all_str[9] : port4_remark;
	
		/* WiFi */
		ssid1_group = all_str[10].length ? all_str[10] : ssid1_group;
		ssid1_remark = all_str[11].length ? all_str[11] : ssid1_remark;
		ssid2_group = all_str[12].length ? all_str[12] : ssid2_group;
		ssid2_remark = all_str[13].length ? all_str[13] : ssid2_remark;
		ssid3_group = all_str[14].length ? all_str[14] : ssid3_group;
		ssid3_remark = all_str[15].length ? all_str[15] : ssid3_remark;
		ssid4_group = all_str[16].length ? all_str[16] : ssid4_group;
		ssid4_remark = all_str[17].length ? all_str[17] : ssid4_remark;
		ssid5_group = all_str[18].length ? all_str[18] : ssid5_group;
		ssid5_remark = all_str[19].length ? all_str[19] : ssid5_remark;
		ssid6_group = all_str[20].length ? all_str[20] : ssid6_group;
		ssid6_remark = all_str[21].length ? all_str[21] : ssid6_remark;
		ssid7_group = all_str[22].length ? all_str[22] : ssid7_group;
		ssid7_remark = all_str[23].length ? all_str[23] : ssid7_remark;
		ssid8_group = all_str[24].length ? all_str[24] : ssid8_group;
		ssid8_remark = all_str[25].length ? all_str[25] : ssid8_remark;
	
		// change the "select element"
		document.getElementById("port0_group").options.selectedIndex = parseInt(port0, 10);
		document.getElementById("port0_remarker").options.selectedIndex = parseInt(port0_remark, 10);
		document.getElementById("port1_group").options.selectedIndex = parseInt(port1, 10);
		document.getElementById("port1_remarker").options.selectedIndex = parseInt(port1_remark, 10);
		document.getElementById("port2_group").options.selectedIndex = parseInt(port2, 10);
		document.getElementById("port2_remarker").options.selectedIndex = parseInt(port2_remark, 10);
		document.getElementById("port3_group").options.selectedIndex = parseInt(port3, 10);
		document.getElementById("port3_remarker").options.selectedIndex = parseInt(port3_remark, 10);
		document.getElementById("port4_group").options.selectedIndex = parseInt(port4, 10);
		document.getElementById("port4_remarker").options.selectedIndex = parseInt(port4_remark, 10);
	
		document.getElementById("ssid1_group").options.selectedIndex = parseInt(ssid1_group, 10);
		document.getElementById("ssid1_remarker").options.selectedIndex = parseInt(ssid1_remark, 10);
		document.getElementById("ssid2_group").options.selectedIndex = parseInt(ssid2_group, 10);
		document.getElementById("ssid2_remarker").options.selectedIndex = parseInt(ssid2_remark, 10);
		document.getElementById("ssid3_group").options.selectedIndex = parseInt(ssid3_group, 10);
		document.getElementById("ssid3_remarker").options.selectedIndex = parseInt(ssid3_remark, 10);
		document.getElementById("ssid4_group").options.selectedIndex = parseInt(ssid4_group, 10);
		document.getElementById("ssid4_remarker").options.selectedIndex = parseInt(ssid4_remark, 10);
		document.getElementById("ssid5_group").options.selectedIndex = parseInt(ssid5_group, 10);
		document.getElementById("ssid5_remarker").options.selectedIndex = parseInt(ssid5_remark, 10);
		document.getElementById("ssid6_group").options.selectedIndex = parseInt(ssid6_group, 10);
		document.getElementById("ssid6_remarker").options.selectedIndex = parseInt(ssid6_remark, 10);
		document.getElementById("ssid7_group").options.selectedIndex = parseInt(ssid7_group, 10);
		document.getElementById("ssid7_remarker").options.selectedIndex = parseInt(ssid7_remark, 10);
		document.getElementById("ssid8_group").options.selectedIndex = parseInt(ssid8_group, 10);
		document.getElementById("ssid8_remarker").options.selectedIndex = parseInt(ssid8_remark, 10);
	
		// show or hide SSID
		ssid1.length ? tr_show("ssid1_tr"):	tr_hide("ssid1_tr");
		document.getElementById("ssid1_td").innerHTML = ssid1 + _("qos wifi") + " (SSID1)";
		ssid2.length ? tr_show("ssid2_tr"):	tr_hide("ssid2_tr");
		document.getElementById("ssid2_td").innerHTML = ssid2 + _("qos wifi") + " (SSID2)";
		ssid3.length ? tr_show("ssid3_tr"):	tr_hide("ssid3_tr");
		document.getElementById("ssid3_td").innerHTML = ssid3 + _("qos wifi") + " (SSID3)";
		ssid4.length ? tr_show("ssid4_tr"):	tr_hide("ssid4_tr");	
		document.getElementById("ssid4_td").innerHTML = ssid4 + _("qos wifi") + " (SSID4)";
		ssid5.length ? tr_show("ssid5_tr"):	tr_hide("ssid5_tr");
		document.getElementById("ssid5_td").innerHTML = ssid5 + _("qos wifi") + " (SSID5)";
		ssid6.length ? tr_show("ssid6_tr"):	tr_hide("ssid6_tr");
		document.getElementById("ssid6_td").innerHTML = ssid6 + _("qos wifi") + " (SSID6)";
		ssid7.length ? tr_show("ssid7_tr"):	tr_hide("ssid7_tr");
		document.getElementById("ssid7_td").innerHTML = ssid7 + _("qos wifi") + " (SSID7)";
		ssid8.length ? tr_show("ssid8_tr"):	tr_hide("ssid8_tr");	
		document.getElementById("ssid8_td").innerHTML = ssid8 + _("qos wifi") + " (SSID8)";

		if(QoSModel == "4" /* remarker only */)
			QoSPortBasedEnableGroupSelect(false);	/* disable all group select */
		else
			QoSPortBasedEnableGroupSelect(true);	/* enable all group select */
	}else{
		div_hide("QoSPortBasedDiv");
	}

}

function QoSPortBasedEnableGroupSelect(opt)
{
	document.getElementById("port0_group").disabled = 
	document.getElementById("port1_group").disabled = 
	document.getElementById("port2_group").disabled =
	document.getElementById("port3_group").disabled =
	document.getElementById("port4_group").disabled = 
/*
	document.getElementById("ssid1_group").disabled = 
	document.getElementById("ssid2_group").disabled = 
	document.getElementById("ssid3_group").disabled = 
	document.getElementById("ssid4_group").disabled = 
	document.getElementById("ssid5_group").disabled = 
	document.getElementById("ssid6_group").disabled = 
	document.getElementById("ssid7_group").disabled = 
	document.getElementById("ssid8_group").disabled =
*/														(!opt); 
}

function QoSHideAllRateCeilSelect()
{
	div_hide("highest_upload_queue_div");
	div_hide("high_upload_queue_div");
	div_hide("default_upload_queue_div");
	div_hide("low_upload_queue_div");
	div_hide("highest_download_queue_div");
	div_hide("high_download_queue_div");
	div_hide("default_download_queue_div");
	div_hide("low_download_queue_div");
}

function QoSShowAllRateCeilSelect()
{
	div_show("highest_upload_queue_div");
	div_show("high_upload_queue_div");
	div_show("default_upload_queue_div");
	div_show("low_upload_queue_div");
	div_show("highest_download_queue_div");
	div_show("high_download_queue_div");
	div_show("default_download_queue_div");
	div_show("low_download_queue_div");
}

function div_hide(ele_str)
{
	mydiv = document.getElementById(ele_str);
	mydiv.style.display = "none";
	mydiv.style.visibility = "hidden";
}

function div_show(ele_str)
{
	mydiv = document.getElementById(ele_str);
	mydiv.style.display = "block";
	mydiv.style.visibility = "visible";
}

/* why not just div by 10? LoL */
function QoSSetupQueue(ele, per)
{
	switch(per){
	case "0":
		ele.options.selectedIndex = 0;	break;
	case "10":
		ele.options.selectedIndex = 1;	break;
	case "20":
		ele.options.selectedIndex = 2;	break;
	case "30":
		ele.options.selectedIndex = 3;	break;
	case "40":
		ele.options.selectedIndex = 4;	break;
	case "50":
		ele.options.selectedIndex = 5;	break;
	case "60":
		ele.options.selectedIndex = 6;	break;
	case "70":
		ele.options.selectedIndex = 7;	break;
	case "80":
		ele.options.selectedIndex = 8;	break;
	case "90":
		ele.options.selectedIndex = 9;	break;
	case "100":
		ele.options.selectedIndex = 10;	break;
	}
}

function UploadQueueShow()
{
	tr_show("title_upload_queue_tr");
	tr_show("highest_upload_queue_tr");
	tr_show("high_upload_queue_tr");
	tr_show("default_upload_queue_tr");
	tr_show("low_upload_queue_tr");
}

function UploadQueueHide()
{
	tr_hide("title_upload_queue_tr");
	tr_hide("highest_upload_queue_tr");
	tr_hide("high_upload_queue_tr");
	tr_hide("default_upload_queue_tr");
	tr_hide("low_upload_queue_tr");
}

function DownloadQueueShow()
{
	tr_show("title_download_queue_tr");
	tr_show("highest_download_queue_tr");
	tr_show("high_download_queue_tr");
	tr_show("default_download_queue_tr");
	tr_show("low_download_queue_tr");
}

function DownloadQueueHide()
{
	tr_hide("title_download_queue_tr");
	tr_hide("highest_download_queue_tr");
	tr_hide("high_download_queue_tr");
	tr_hide("default_download_queue_tr");
	tr_hide("low_download_queue_tr");
}

function tr_show(ele_str)
{
		document.getElementById(ele_str).style.visibility = "visible";
		document.getElementById(ele_str).style.display = tr_style_display_on();
}

function tr_hide(ele_str)
{
		document.getElementById(ele_str).style.visibility = "hidden";
		document.getElementById(ele_str).style.display = "none";
}

function tr_style_display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari,...
		return "table-row";
	}
}

function QoSSetupCheck()
{
	if(document.QoSSetup.QoSSelect.options.selectedIndex == 0){
		;  // do nothing
	}else if(	document.QoSSetup.QoSSelect.options.selectedIndex == 1 /* bi-dir*/ ||
				document.QoSSetup.QoSSelect.options.selectedIndex == 2 /* upload */ || 
				document.QoSSetup.QoSSelect.options.selectedIndex == 3 /* download */ ||
				document.QoSSetup.QoSSelect.options.selectedIndex == 4 /* download */ ){
		if(document.QoSSetup.UploadBandwidth.value == "custom"){
			if(document.QoSSetup.UploadBandwidth_Custom.value == ""){
				alert("Please fill the upload bandwidth.");
				return false;
			}
			if(!checkNum(document.QoSSetup.UploadBandwidth_Custom.value)){
				alert("The upload bandwidth format is wrong. (ex. \"10k\" \"20M\")");
				return false;
			}
				
			var value = getTrueValue(document.QoSSetup.UploadBandwidth_Custom.value);
			if( value  > getTrueValue(QOS_MAX_HARD_LIMITED) ){
				alert("The value of upload bandwidth is too large.(" + QOS_MAX_HARD_LIMITED + ")");
				return false;
			}
			if( value < QOS_MIN_HARD_LIMITED ){
				var ret = confirm("The value of upload bandwidth is too small, are you sure?");
				if(ret == false)
					return false;
			}
		}
		if(document.QoSSetup.DownloadBandwidth.value == "custom"){
			if(document.QoSSetup.DownloadBandwidth_Custom.value == ""){
				alert("Please fill the down bandwidth.");
				return false;
			}
			if(!checkNum(document.QoSSetup.DownloadBandwidth_Custom.value)){
				alert("The download bandwidth format is wrong. (ex. \"10k\" \"20M\")");
				return false;
			}

			var value = getTrueValue(document.QoSSetup.DownloadBandwidth_Custom.value);
			if( value <= 0 ){
				alert("The value of download bandwidth is too small.");
				return false;
			}
		}
	}
	if( DownloadQueueRateCheck() == false ||
		UploadQueueRateCheck() == false	||
		DownloadQueueCeilCheck() == false ||
		UploadQueueCeilCheck() == false)
		return false;

	return true;
}

function QoSSelectChange()
{
	var QoSSetup = document.QoSSetup.QoSSelect.options.selectedIndex;
	QoSModel_idx = document.QoSSetup.Model.options.selectedIndex;

	UploadQueueHide();
	DownloadQueueHide();
	QoSHideAllRateCeilSelect();

	hideULRule();
	hideDLRule();
	div_hide("QoSPortBasedDiv");

	if(QoSSetup == 1 && QoS == "1"){
		showULRule();
		showDLRule();
	}else if(QoSSetup == 2 && QoS == "2"){
		showULRule();
	}else if(QoSSetup == 3 && QoS == "3"){
		showDLRule();
	}else if(QoSSetup == 4 && QoS == "4"){
		div_show("QoSPortBasedDiv");
	}

	if(QoSSetup == 0 /* disable */){
		document.QoSSetup.UploadBandwidth.disabled = true;
		document.QoSSetup.DownloadBandwidth.disabled = true;
		document.QoSSetup.Model.disabled = true;
		document.QoSSetup.UploadBandwidth_Custom.disabled = true;
		document.QoSSetup.DownloadBandwidth_Custom.disabled = true;
		document.QoSSetup.ReserveBandwidth.disabled = true;
	}else{
		document.QoSSetup.Model.disabled = false;
		document.QoSSetup.UploadBandwidth.disabled = false;
		LoadUploadBW();
		document.QoSSetup.DownloadBandwidth.disabled = false;
		LoadDownloadBW();
		document.QoSSetup.ReserveBandwidth.disabled = false;

		if( QoSSetup == 1 /* Bi */ ){
			UploadQueueShow();
			DownloadQueueShow();

			updateRateCeil(QoSModel_idx);
		}else if( QoSSetup == 2 /* Upload */) {
			UploadQueueShow();
			updateRateCeil(QoSModel_idx);

		}else if( QoSSetup == 3 /* Download */) {
			DownloadQueueShow();
			updateRateCeil(QoSModel_idx);
		}else if( QoSSetup == 4 /* port based */) {
			UploadQueueShow();
			updateRateCeil(QoSModel_idx);

//			div_show("QoSPortBasedDiv");
//			if(QoSModel_idx == 3 /* remarker only */)
//				QoSPortBasedEnableGroupSelect(false);	/* disable all group select */
//			else
//				QoSPortBasedEnableGroupSelect(true);	/* enable all group select */
		}
	}
}

function updateRateCeil(idx)
{
	if(QoSModel_idx == 0 /* DRR */ ){
		QoSShowAllRateCeilSelect();
	}else if(QoSModel_idx == 1 /* SPQ */ ){
		QoSHideAllRateCeilSelect();
	}else if(QoSModel_idx == 2 /* SPQ+DRR*/ ){
		div_hide("highest_upload_queue_div");
		div_hide("high_upload_queue_div");
		div_show("default_upload_queue_div");
		div_show("low_upload_queue_div");
		div_hide("highest_download_queue_div");
		div_hide("high_download_queue_div");
		div_show("default_download_queue_div");
		div_show("low_download_queue_div");
	}else if(QoSModel_idx == 3 /* Remark Only */ ){
		QoSHideAllRateCeilSelect();
		UploadQueueHide();
		DownloadQueueHide();
	}
}

function LoadUploadBW()
{
	if(document.QoSSetup.UploadBandwidth.options.selectedIndex == 0){
		document.QoSSetup.UploadBandwidth_Custom.value = uploadBandwidth_custom;
		document.QoSSetup.UploadBandwidth_Custom.disabled = false;
		document.QoSSetup.UploadBandwidth_Custom.style.display = '';
	}else{
		document.QoSSetup.UploadBandwidth_Custom.disabled = true;
		document.QoSSetup.UploadBandwidth_Custom.style.display = 'none';
	    if(uploadBandwidth != "custom"){
	  		for(var i=0; i< document.QoSSetup.UploadBandwidth.length; i++){
	  			if(document.QoSSetup.UploadBandwidth.options[i].value == uploadBandwidth){
	  				document.QoSSetup.UploadBandwidth.options.selectedIndex = i;
	  				break;
	  			}
	  		}
	    }
	}
}

function UploadBWChange()
{
	if(document.QoSSetup.UploadBandwidth.options.selectedIndex == 0){
		document.QoSSetup.UploadBandwidth_Custom.value = uploadBandwidth_custom;
		document.QoSSetup.UploadBandwidth_Custom.disabled = false;
		document.QoSSetup.UploadBandwidth_Custom.style.display = '';
	}else{
		document.QoSSetup.UploadBandwidth_Custom.disabled = true;
		document.QoSSetup.UploadBandwidth_Custom.style.display = 'none';
	}
}

function LoadDownloadBW()
{
	if(document.QoSSetup.DownloadBandwidth.options.selectedIndex == 0){
		document.QoSSetup.DownloadBandwidth_Custom.value = downloadBandwidth_custom;
		document.QoSSetup.DownloadBandwidth_Custom.disabled = false;
		document.QoSSetup.DownloadBandwidth_Custom.style.display = '';
	}else{
		document.QoSSetup.DownloadBandwidth_Custom.disabled = true;
		document.QoSSetup.DownloadBandwidth_Custom.style.display = 'none';
		if(downloadBandwidth != "custom"){
			for(var i=0; i< document.QoSSetup.DownloadBandwidth.length; i++){
				if(document.QoSSetup.DownloadBandwidth.options[i].value == downloadBandwidth){
					document.QoSSetup.DownloadBandwidth.options.selectedIndex = i;
					break;
				}
			}
		}
	}
}

function DownloadBWChange()
{
	if(document.QoSSetup.DownloadBandwidth.options.selectedIndex == 0){
		document.QoSSetup.DownloadBandwidth_Custom.value = downloadBandwidth_custom;
		document.QoSSetup.DownloadBandwidth_Custom.disabled = false;
		document.QoSSetup.DownloadBandwidth_Custom.style.display = '';
	}else{
		document.QoSSetup.DownloadBandwidth_Custom.disabled = true;
		document.QoSSetup.DownloadBandwidth_Custom.style.display = 'none';
	}
}

function PrintRules(Rules, dir)
{
	if(Rules == "")
		return;
	var a_rule = new Array();

	a_rule = Rules.split(";");
	for(i = 0; i < a_rule.length; i++){
		var j=0;
		var desc = "";
		var entry = new Array();
		entry = a_rule[i].split(",");

		var name = entry[j++];
		var af_index = entry[j++];
		var dp_index = entry[j++];
		var mac_address = entry[j++];
		if(mac_address.length)
			desc += _("qos classifier mac") + ": "+ mac_address + "<br>";

		var protocol = entry[j++];
		if(protocol.length)
			desc += _("qos classifier proto") + ": " + protocol + "<br>";

		var dip = entry[j++];
		if(dip.length)
			desc += _("qos classifier dip") + ": " + dip + "<br>";

		var sip = entry[j++];
		if(sip.length)
			desc += _("qos classifier sip") + ": " + sip + "<br>";

		var pkt_len_from = entry[j++];
		var pkt_len_to = entry[j++];
		if(pkt_len_from.length && pkt_len_to.length)
			desc += _("qos classifier pktlen") + ": "+ pkt_len_from + " - " + pkt_len_to + "<br>";

		var dst_port_from = entry[j++];
		var dst_port_to = entry[j++];
		if(dst_port_from.length && dst_port_to.length){
			desc += _("qos classifier dport") + ": " + dst_port_from + " - " + dst_port_to + "<br>";
		}else if (dst_port_from.length){
			desc += _("qos classifier dport") + ": " +dst_port_from + "<br>";
		}

		var src_port_from  = entry[j++];
		var src_port_to  = entry[j++];
		if(src_port_from.length && src_port_to.length){
			desc += _("qos classifier sport") + ": " + src_port_from + " - " + src_port_to + "<br>";
		}else if (src_port_from.length){
			desc += _("qos classifier sport") + ": " + src_port_from + "<br>";
		}

		var layer7 = entry[j++];
		if(layer7.length)
			desc += _("qos classifier l7") + ": " + layer7 + "<br>";

		var dscp = entry[j++];
		if(dscp.length)
			desc += _("qos classifier dscp") + ": " + dscp + "<br>";

		var ingress_if  = entry[j++];
		if(ingress_if.length)
			desc += "Ingress Interface: " + ingress_if + "<br>";

		var remarker = entry[j++];
		if(remarker.length && remarker != "N/A")
			desc += _("qos classifier remark") + ": " + remarker + "<br>";

		document.write("<tr class=" + ((dir=='UL') ? "tr_qos1" : "tr_qos2")  + "><td>"+ (i+1) +"<input type=checkbox name=del_qos_" + i + "> </td> <td>" + name + "</td> <td>" + getGroupNameByIndex(af_index) + "</td>  <td>" + desc + "</td>  </tr>");
	}
}


function getTrueValue(str)
{
	var rc;
	rc = parseInt(str);
	if(str.charAt(str.length-1) == 'k' || str.charAt(str.length-1) == 'K')
		rc = rc * 1024;
	else if(str.charAt(str.length-1) == 'm' || str.charAt(str.length-1) == 'M')
		rc = rc * 1024 * 1024;
	return rc;
}

function checkNum(str)
{
	if(str.length < 2)
		return false;
	for(i=0; i<str.length-1; i++){
		if( str.charAt(i) >= '0' && str.charAt(i) <= '9')
			continue;
		return false;
	}
	if( str.charAt(str.length-1) == 'k' || str.charAt(str.length-1) == 'K' ||
		str.charAt(str.length-1) == 'm' || str.charAt(str.length-1) == 'M' || 
		(str.charAt(str.length-1) >= '0' && str.charAt(str.length-1) <= '9') )
		return true;
	return false;
}

function UploadQueueRateCheck()
{
	var total = 0;
	if(QoSModel == 1 /* DRR */){
		total =	parseInt(document.QoSSetup.highest_upload_queue_rate.value) +
				parseInt(document.QoSSetup.high_upload_queue_rate.value) +
				parseInt(document.QoSSetup.default_upload_queue_rate.value) +
				parseInt(document.QoSSetup.low_upload_queue_rate.value);
	}else if(QoSModel == 2 /* SPQ */){
	}else if(QoSModel == 3 /* SPQ + DRR */){
		total = parseInt(document.QoSSetup.default_upload_queue_rate.value) +
				parseInt(document.QoSSetup.low_upload_queue_rate.value);
	}else if(QoSModel == 4 /* remark only */){
	}	
	if(total > 100){
		alert("The sum of upload queues' rate bandwidth is exceed 100%!");
		return false;
	}
	return true;
}

function UploadQueueCeilCheck()
{
	var highest_rate = parseInt(document.QoSSetup.highest_upload_queue_rate.value);
	var highest_ceil = parseInt(document.QoSSetup.highest_upload_queue_ceil.value);
	var high_rate = parseInt(document.QoSSetup.high_upload_queue_rate.value);
	var high_ceil = parseInt(document.QoSSetup.high_upload_queue_ceil.value);
	var default_rate = parseInt(document.QoSSetup.default_upload_queue_rate.value);
	var default_ceil = parseInt(document.QoSSetup.default_upload_queue_ceil.value);
	var low_rate = parseInt(document.QoSSetup.low_upload_queue_rate.value);
	var low_ceil = parseInt(document.QoSSetup.low_upload_queue_ceil.value);

	if(QoSModel == 1 /* DRR */){
		if(	highest_rate > highest_ceil || high_rate > high_ceil || default_rate > default_ceil || low_rate > low_ceil){
			alert("The Rate value can't large than Ceil.");
			return false;
		}

	}else if(QoSModel == 2 /* SPQ */){
	}else if(QoSModel == 3 /* SPQ + DRR */){
		if(	default_rate > default_ceil || low_rate > low_ceil){
			alert("The Rate value can't large than Ceil.");
			return false;
		}
	}else if(QoSModel == 4 /* remark only */){
	}	
	return true;
}

function DownloadQueueCeilCheck()
{
	var highest_rate = parseInt(document.QoSSetup.highest_download_queue_rate.value);
	var highest_ceil = parseInt(document.QoSSetup.highest_download_queue_ceil.value);
	var high_rate = parseInt(document.QoSSetup.high_download_queue_rate.value);
	var high_ceil = parseInt(document.QoSSetup.high_download_queue_ceil.value);
	var default_rate = parseInt(document.QoSSetup.default_download_queue_rate.value);
	var default_ceil = parseInt(document.QoSSetup.default_download_queue_ceil.value);
	var low_rate = parseInt(document.QoSSetup.low_download_queue_rate.value);
	var low_ceil = parseInt(document.QoSSetup.low_download_queue_ceil.value);

	if(QoSModel == 1 /* DRR */){
		if(	highest_rate > highest_ceil || high_rate > high_ceil || default_rate > default_ceil || low_rate > low_ceil){
			alert("The Rate value can't large than Ceil.");
			return false;
		}

	}else if(QoSModel == 2 /* SPQ */){
	}else if(QoSModel == 3 /* SPQ + DRR */){
		if(	default_rate > default_ceil || low_rate > low_ceil){
			alert("The Rate value can't large than Ceil.");
			return false;
		}
	}else if(QoSModel == 4 /* remark only */){
	}	
	return true;
}

function DownloadQueueRateCheck()
{
	var total = 0;
	if(QoSModel == 1 /* DRR */){
		total =	parseInt(document.QoSSetup.highest_download_queue_rate.value) +
				parseInt(document.QoSSetup.high_download_queue_rate.value) +
				parseInt(document.QoSSetup.default_download_queue_rate.value) +
				parseInt(document.QoSSetup.low_download_queue_rate.value);
	}else if(QoSModel == 2 /* SPQ */){
	}else if(QoSModel == 3 /* SPQ + DRR */){
		total =	parseInt(document.QoSSetup.default_download_queue_rate.value) +
				parseInt(document.QoSSetup.low_download_queue_rate.value);
	}else if(QoSModel == 4 /* remark only */){
	}	
	if(total > 100){
		alert("The sum of download queues' rate bandwidth is exceed 100%!");
		return false;
	}
	return true;
}




</script>
</head>
<!--     body      -->
<body onload="onInit()">
<table class="body"><tbody><tr><td>
<h1 id="QoSTitleStr">Quality of Service Settings </h1>
<p id="QoSIntroStr"> You may setup rules to provide Quality of Service guarantees for specific applications.</p>
<hr>

<form method="post" name="trans" action="">
<input name="ModifyAFDir" value="" type="hidden">
<input name="ModifyAFIndex" value="" type="hidden">
<input name="ModifyDPIndex" value="" type="hidden">
<input name="ModifyAFName" value="" type="hidden">
<input name="ModifyAFRate" value="" type="hidden">
<input name="ModifyAFCeil" value="" type="hidden">
</form>

<form method="post" name="trans_addrule" action="">
<input name="AddruleDir" value="" type="hidden">
</form>




<form method="post" name="QoSSetup" action="/goform/QoSSetup">
<table border=1 bordercolor=#9BABBD width="540">
<tr>
	<td class="title" colspan="2" id="QoSSetupStr">QoS Setup </td>
</tr>
<tr>
	<td class="head" id="QoSStr">
		Quality of Service
	</td>

	<td>
	<select name="QoSSelect" size="1" onChange="QoSSelectChange()">
	<option value=0 id="QoSDisableStr">Disable</option>
	<option value=1 id="QoSEnableBidirectionStr">Enable - Bidirection</option>
	<option value=2 id="QoSEnableUploadOnlyStr">Enable - Upload QoS Only </option>
	<option value=3 id="QoSEnableDownloadOnlyStr">Enable - Download QoS Only</option>
	<script language="JavaScript" type="text/javascript">
		if(isPortBasedQoSSupport == "1")
			document.write("<option value=4 id=QoSPortBasedOnlyStr>" + _("qos portbased qos")+ "</option>");
	</script>
	</select>
	</td>
</tr>
<tr>
	<td class="head" id="QoSUpBWStr">
		Upload Bandwidth:
	</td>
	<td>
		<select name="UploadBandwidth" id="UploadBandwidth" size="1" onChange="UploadBWChange()">
		<option value="custom" id="QoSBWCustomStr">User defined</option>
		<option value="64k">64k</option>
		<option value="96k">96k</option>
		<option value="128k">128k</option>
		<option value="192k">192k</option>
		<option value="256k">256k</option>
		<option value="384k" >384k</option>
		<option value="512k">512k</option>
		<option value="768k">768k</option>
		<option value="1M">1M</option>
		<option value="2M">2M</option>
		<option value="4M">4M</option>
		<option value="8M">8M</option>
		<option value="10M">10M</option>
		<option value="12M">12M</option>
		<option value="16M">16M</option>
		<option value="20M">20M</option>
		<option value="24M">24M</option>
		<option value="32M">32M</option>
		<option value="50M">50M</option>
		</select>

		<input type=text name=UploadBandwidth_Custom size=6 maxlength=8 style="display:none"> Bits/sec
	</td>
</tr>
<tr>
	<td class="head" id="QoSDownBWStr">
		Download Bandwidth:
	</td>
	<td>
		<select name="DownloadBandwidth" id="DownloadBandwidth" size="1" onChange="DownloadBWChange()">
		<option value="custom" id="QoSBWCustomStr2">User defined</option>
		<option value="64k">64k</option>
		<option value="96k">96k</option>
		<option value="128k">128k</option>
		<option value="192k">192k</option>
		<option value="256k">256k</option>
		<option value="384k" >384k</option>
		<option value="512k">512k</option>
		<option value="768k">768k</option>
		<option value="1M">1M</option>
		<option value="2M">2M</option>
		<option value="4M">4M</option>
		<option value="8M">8M</option>
		<option value="10M">10M</option>
		<option value="12M">12M</option>
		<option value="16M">16M</option>
		<option value="20M">20M</option>
		<option value="24M">24M</option>
		<option value="32M">32M</option>
		<option value="50M">50M</option>
		</select>

		<input type=text name=DownloadBandwidth_Custom size=6 maxlength=8 style="display:none"> Bits/sec
	</td>
</tr>

<tr>
	<td class="head" id="QoSModelStr">
		QoSModel:
	</td>
	<td>
		<select name="Model" id="Model" size="1" onchange="QoSSelectChange()">
		<option value="1">DRR</option>
		<option value="2">SPQ</option>
		<option value="3">SPQ+DRR</option>
		<option value="4">Remark only</option>
		</select>
	</td>
</tr>

<tr>
	<td class="head" id="ReserveBandwidthStr">
		Reserved Bandwidth:
	</td>
	<td>
		<select name="ReserveBandwidth" id="ReserveBandwidth" size="1">
		<option value="0">0%</option>
		<option value="10">10%</option>
		<option value="20">20%</option>
		<option value="30">30%</option>
		<option value="40">40%</option>
		</select>	(10% is recommanded)
	</td>
</tr>

<!-- Upload -->
<tr style="visibility: visible; display: table-row;" id="title_upload_queue_tr">
	<td class="title" colspan="2" id="QoSUploadTitleStr">QoS Upload Settings</td>
</tr>

<!-- EF / AF5 -->
<tr style="visibility: visible; display: table-row;" id="highest_upload_queue_tr" class="tr_qos1">
	<td id="QoSUploadHighestStr">
			Highest
	</td>
	<td>
	<div style="display: block; visibility: visible;" id="highest_upload_queue_div">
		Rate:
		<select name="highest_upload_queue_rate" id="highest_upload_queue_rate" size="1" onchange="UploadQueueRateCheck()">
			<option value="0">0%</option>
			<option selected="selected" value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
		Ceil:
		<select name="highest_upload_queue_ceil" id="highest_upload_queue_ceil" size="1" onchange="UploadQueueCeilCheck()">
			<option value="0">0%</option>
			<option value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option selected="selected" value="100">100%</option>
		</select>
	</div>
	</td>

</tr>

<!-- AF2 -->
<tr style="visibility: visible; display: table-row;" class="tr_qos1" id="high_upload_queue_tr">
	<td id="QoSUploadHighStr">
			High
	</td>
	<td>
	<div style="display: block; visibility: visible;" id="high_upload_queue_div">
		Rate:
		<select name="high_upload_queue_rate" id="high_upload_queue_rate" size="1" onchange="UploadQueueRateCheck()">
			<option value="0">0%</option>
			<option selected="selected" value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
		Ceil:
		<select name="high_upload_queue_ceil" id="high_upload_queue_ceil" size="1" onchange="UploadQueueCeilCheck()">
			<option selected="selected" value="0">0%</option>
			<option value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
	</div>
	</td>
</tr>

<!-- Default/AF6 -->
<tr style="visibility: visible; display: table-row;" class="tr_qos1" id="default_upload_queue_tr">
	<td id="QoSUploadDefaultStr">
			Default
	</td>
	<td>
	<div style="display: block; visibility: visible;" id="default_upload_queue_div">
		Rate:
		<select name="default_upload_queue_rate" id="default_upload_queue_rate" size="1" onchange="UploadQueueRateCheck()">
			<option value="0">0%</option>
			<option selected="selected" value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
		Ceil:
		<select name="default_upload_queue_ceil" id="default_upload_queue_ceil" size="1" onchange="UploadQueueCeilCheck()">
			<option selected="selected" value="0">0%</option>
			<option value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
	</div>
	</td>
</tr>

<!-- AF1 -->
<tr style="visibility: visible; display: table-row;" class="tr_qos1" id="low_upload_queue_tr">
	<td id="QoSUploadLowStr">
			Low
	</td>
<td>
	<div style="display: block; visibility: visible;" id="low_upload_queue_div">
		Rate:
		<select name="low_upload_queue_rate" id="low_upload_queue_rate" size="1" onchange="UploadQueueRateCheck()">
			<option value="0">0%</option>
			<option selected="selected" value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
		Ceil:
		<select name="low_upload_queue_ceil" id="low_upload_queue_ceil" size="1" onchange="UploadQueueCeilCheck()">
			<option selected="selected" value="0">0%</option>
			<option value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
	</div>
	</td>
</tr>



<!-- Download -->
<tr style="visibility: visible; display: table-row;" id="title_download_queue_tr">
	<td class="title" colspan="2" id="QoSDownloadTitleStr">QoS Download Settings</td>
</tr>

<!-- EF / AF5 -->
<tr style="visibility: visible; display: table-row;" class="tr_qos2" id="highest_download_queue_tr">
	<td id="QoSDownloadHighestStr">
			Highest
	</td>
	<td>
	<div style="display: block; visibility: visible;" id="highest_download_queue_div">
		Rate:
		<select name="highest_download_queue_rate" id="highest_download_queue_rate" size="1" onchange="DownloadQueueRateCheck()">
			<option value="0">0%</option>
			<option selected="selected" value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
		Ceil:
		<select name="highest_download_queue_ceil" id="highest_download_queue_ceil" size="1" onchange="DownloadQueueCeilCheck()">
			<option selected="selected" value="0">0%</option>
			<option value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
	</div>
	</td>
</tr>

<!-- AF2 -->
<tr style="visibility: visible; display: table-row;" class="tr_qos2" id="high_download_queue_tr">
	<td id="QoSDownloadHighStr">
			High
	</td>
	<td>
	<div style="display: block; visibility: visible;" id="high_download_queue_div">
		Rate:
		<select name="high_download_queue_rate" id="high_download_queue_rate" size="1" onchange="DownloadQueueRateCheck()">
			<option value="0">0%</option>
			<option selected="selected" value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
		Ceil:
		<select name="high_download_queue_ceil" id="high_download_queue_ceil" size="1" onchange="DownloadQueueCeilCheck()">
			<option selected="selected" value="0">0%</option>
			<option value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
	</div>
	</td>
</tr>

<!-- Default/AF6 -->
<tr style="visibility: visible; display: table-row;" class="tr_qos2" id="default_download_queue_tr">
	<td id="QoSDownloadDefaultStr">
			Default
	</td>
	<td>
	<div style="display: block; visibility: visible;" id="default_download_queue_div">
		Rate:
		<select name="default_download_queue_rate" id="default_download_queue_rate" size="1" onchange="DownloadQueueRateCheck()">
			<option value="0">0%</option>
			<option selected="selected" value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
		Ceil:
		<select name="default_download_queue_ceil" id="default_download_queue_ceil" size="1" onchange="DownloadQueueCeilCheck()">
			<option selected="selected" value="0">0%</option>
			<option value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
	</div>
	</td>
</tr>

<!-- AF1 -->
<tr style="visibility: visible; display: table-row;" class="tr_qos2" id="low_download_queue_tr">
	<td id="QoSDownloadLowStr">
			Low
	</td>
	<td>
	<div style="display: block; visibility: visible;" id="low_download_queue_div">
		Rate:
		<select name="low_download_queue_rate" id="low_download_queue_rate" size="1" onchange="DownloadQueueRateCheck()">
			<option value="0">0%</option>
			<option selected="selected" value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
		Ceil:
		<select name="low_download_queue_ceil" id="low_download_queue_ceil" size="1" onchange="DownloadQueueCeilCheck()">
			<option selected="selected" value="0">0%</option>
			<option value="10">10%</option>
			<option value="20">20%</option>
			<option value="30">30%</option>
			<option value="40">40%</option>
			<option value="50">50%</option>
			<option value="60">60%</option>
			<option value="70">70%</option>
			<option value="80">80%</option>
			<option value="90">90%</option>
			<option value="100">100%</option>
		</select>
	</div>
	</td>
</tr>

</table>
<input value="Submit" id="QoSSetupSubmitStr" name="QoSSetupSubmitStr" onclick="return QoSSetupCheck()" type="submit"> &nbsp;&nbsp;
<!--
<input value="Reset" id="QoSSetupResetStr" name="QoSSetupResetStr" type="reset">
-->
</form>

<br>



<!--
	QoS Port Based
-->
<div id="QoSPortBasedDiv">

<form method="post" name="QoSPortSetup" action="/goform/QoSPortSetup">
<table border="1" bordercolor="#9babbd" width="540" id="QoSPortSetupTable" name ="QoSPortSetupTable">
<tbody><tr>
	<td class="title" colspan="2" id="QoSPortSetupStr">QoS Port Setup</td>
</tr>

<tr>
	<td class="head" id="QoSPort0Str">
			Port 0
	</td>
	<td>
		Group:
		<select name="port0_group" id="port0_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="port0_remarker" id="port0_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>
<tr>
	<td class="head" id="QoSPort1Str">
			Port 1
	</td>
	<td>
		Group:
		<select name="port1_group" id="port1_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="port1_remarker" id="port1_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr>
	<td class="head" id="QoSPort2Str">
			Port 2
	</td>
	<td>
		Group:
		<select name="port2_group" id="port2_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="port2_remarker" id="port2_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr>
	<td class="head" id="QoSPort3Str">
			Port 3
	</td>
	<td>
		Group:
		<select name="port3_group" id="port3_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="port3_remarker" id="port3_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
</td>
</tr>

<tr>
	<td class="head" id="QoSPort4Str">
			Port 4
	</td>
	<td>
		Group:
		<select name="port4_group" id="port4_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="port4_remarker" id="port4_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr id="ssid1_tr">
	<td class="head" id="ssid1_td">
	</td>
	<td>
		Group:
		<select name="ssid1_group" id="ssid1_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="ssid1_remaker" id="ssid1_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr id="ssid2_tr">
	<td class="head" id="ssid2_td">
	</td>
	<td>
		Group:
		<select name="ssid2_group" id="ssid2_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="ssid2_remaker" id="ssid2_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr id="ssid3_tr">
	<td class="head" id="ssid3_td">
	</td>
	<td>
		Group:
		<select name="ssid3_group" id="ssid3_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="ssid3_remaker" id="ssid3_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr id="ssid4_tr">
	<td class="head" id="ssid4_td">
	</td>
	<td>
		Group:
		<select name="ssid4_group" id="ssid4_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="ssid4_remaker" id="ssid4_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr id="ssid5_tr">
	<td class="head" id="ssid5_td">
	</td>
	<td>
		Group:
		<select name="ssid5_group" id="ssid5_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="ssid5_remaker" id="ssid5_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr id="ssid6_tr">
	<td class="head" id="ssid6_td">
	</td>
	<td>
		Group:
		<select name="ssid6_group" id="ssid6_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="ssid6_remaker" id="ssid6_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr id="ssid7_tr">
	<td class="head" id="ssid7_td">
	</td>
	<td>
		Group:
		<select name="ssid7_group" id="ssid7_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="ssid7_remaker" id="ssid7_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

<tr id="ssid8_tr">
	<td class="head" id="ssid8_td">
	</td>
	<td>
		Group:
		<select name="ssid8_group" id="ssid8_group" size="1">
			<option value="0"><script> document.write(AF5ULName) </script></option>
			<option value="1"><script> document.write(AF2ULName) </script></option>
			<option value="2"><script> document.write(AF6ULName) </script></option>
			<option value="3"><script> document.write(AF1ULName) </script></option>
		</select>
		Remarker:
		<select name="ssid8_remaker" id="ssid8_remarker">
		<option value="0">Not change</option>
		<option value="1">BE</option>
		<option value="2">AF11</option>
		<option value="3">AF12</option>
		<option value="4">AF13</option>
		<option value="5">AF21</option>
		<option value="6">AF22</option>
		<option value="7">AF23</option>
		<option value="8">AF31</option>
		<option value="9">AF32</option>
		<option value="10">AF33</option>
		<option value="11">AF41</option>
		<option value="12">AF42</option>
		<option value="13">AF43</option>
		<option value="14">EF</option>
		</select>
	</td>
</tr>

</tbody>
</table>
<input value="Submit" id="QoSPortSetupSubmitStr" name="QoSPortSetupSubmitStr" type="submit"> &nbsp;&nbsp;
<!--
<input value="Reset" id="QoSPortSetupResetStr" name="QoSPortSetupResetStr" type="reset">
-->
</form>
</div>


<table>
<tbody><tr>
<td>


<table valign="top" align="left">
<tbody><tr>
<td style="visibility: visible; display: table;" id="div_qos_ul_group_test" align="left" valign="top">

<form method="post" name="QoSDeleteULRules" action="/goform/QoSDeleteULRules">
<table id="div_qos_ul_rules" name="div_qos_ul_rules" style="visibility: visible; display: table;" border="1" bordercolor="#9babbd" width="400">
<tbody>
<tr style="visibility: visible; display: table-row;">
	<td class="title" colspan="4">QoS Upload Rule Settings</td>
</tr>
<tr>
	<td class="title" id="QoSRuleNoStr" width="20">No</td>
	<td class="title" id="QoSRuleNameStr" width="50">Name</td>
	<td class="title" id="QoSRulePrioStr" width="30">Group</td>
	<td class="title" id="QoSRuleInfoStr" width="200">Info.</td>
</tr>

<script language="JavaScript" type="text/javascript">
	PrintRules(ULRules, 'UL');
</script>
</tbody></table>
<input style="" id="QoSRuleAddStr" name="add_ul_rule" value="Add" onclick="AddRule('UL');" type="button">
<input style="" id="QoSRuleDelStr" name="del_ul_rule" value="Delete" type="submit">
</form>

</td>
<td id="div_qos_dl_group_test" align="left" valign="top">

<form method="post" name="QoSDeleteDLRules" action="/goform/QoSDeleteDLRules">
<table id="div_qos_dl_rules" name="div_qos_dl_rules" style="visibility: hidden; display: none;" border="1" bordercolor="#9babbd" width="400">
<tbody>
<tr style="visibility: visible; display: table-row;">
	<td class="title" colspan="4">QoS Download Rule Settings</td>
</tr>
<tr>
	<td class="title" id="QoSRuleNoStr2" width="20">  No. </td>
	<td class="title" id="QoSRuleNameStr2" width="50">  Name. </td>
	<td class="title" id="QoSRulePrioStr2" width="30">  Prio. </td>
	<td class="title" id="QoSRuleInfoStr2" width="200">  Info. </td>
</tr>

<script language="JavaScript" type="text/javascript">
	PrintRules(DLRules, 'DL');
</script>
</tbody></table>
<input style="display: none;" id="QoSDLRuleAddStr" name="add_dl_rule" value="qos rule dl add" onclick="AddRule('DL');" type="button">
<input style="display: none;" id="QoSDLRuleDelStr" name="del_dl_rule" value="qos rule dl del" type="submit">
</form>

</td></tr>
</tbody></table>


<table style="display: table; visibility: visible;" id="div_qos_loaddefault">
<form method="post" name="QoSLoadDefault" action="/goform/QoSLoadDefaultProfile"></form>
<tbody><tr><td>
<!--
	<input name="QoSLoadProfileStr" id="QoSLoadProfileStr" value="Load default" type="submit">
-->
</td></tr>

</tbody></table>

</td></tr></tbody></table>
</td></tr></tbody></table></body></html>
