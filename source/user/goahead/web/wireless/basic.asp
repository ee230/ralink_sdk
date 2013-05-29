<!-- Copyright 2004, Ralink Technology Corporation All Rights Reserved. -->
<html>
<head>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Expires" CONTENT="-1">
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<title>Basic Wireless Settings</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

var PhyMode  = '<% getCfgZero(1, "WirelessMode"); %>';
var HiddenSSID  = '<% getCfgZero(1, "HideSSID"); %>';
var APIsolated = '<% getCfgZero(1, "NoForwarding"); %>';
var mbssidapisolated = '<% getCfgZero(1, "NoForwardingBTNBSSID"); %>';
var ChIdx  = '<% getWlanChannel(); %>';
var fxtxmode = '<% getCfgGeneral(1, "FixedTxMode"); %>';
var CntyCd = '<% getCfgGeneral(1, "CountryCode"); %>';
var ht_mode = '<% getCfgZero(1, "HT_OpMode"); %>';
var ht_bw = '<% getCfgZero(1, "HT_BW"); %>';
var ht_gi = '<% getCfgZero(1, "HT_GI"); %>';
var ht_stbc = '<% getCfgZero(1, "HT_STBC"); %>';
var ht_mcs = '<% getCfgZero(1, "HT_MCS"); %>';
var ht_htc = '<% getCfgZero(1, "HT_HTC"); %>';
var ht_rdg = '<% getCfgZero(1, "HT_RDG"); %>';
var ht_extcha = '<% getCfgZero(1, "HT_EXTCHA"); %>';
var ht_amsdu = '<% getCfgZero(1, "HT_AMSDU"); %>';
var ht_autoba = '<% getCfgZero(1, "HT_AutoBA"); %>';
var ht_badecline = '<% getCfgZero(1, "HT_BADecline"); %>';
var ht_disallow_tkip = '<% getCfgZero(1, "HT_DisallowTKIP"); %>';
var ht_2040_coexit = '<% getCfgZero(1, "HT_BSSCoexistence"); %>';
var ht_f_40mhz = '<% getCfgZero(1, "HT_40MHZ_INTOLERANT"); %>';
var apcli_include = '<% getWlanApcliBuilt(); %>';
var draft3b = '<% getRax11nDraft3Built(); %>';
var mesh_include = '<% getMeshBuilt(); %>';
var tx_stream_idx = '<% getCfgZero(1, "HT_TxStream"); %>';
var rx_stream_idx = '<% getCfgZero(1, "HT_RxStream"); %>';
var txrxStream = '<% getRaxHTStream(); %>';
var max_bssid_num = '<% getMaxBssidNum(); %>';

ChLst_24G = new Array(14);
ChLst_24G[0] = "2412MHz (Channel 1)";
ChLst_24G[1] = "2417MHz (Channel 2)";
ChLst_24G[2] = "2422MHz (Channel 3)";
ChLst_24G[3] = "2427MHz (Channel 4)";
ChLst_24G[4] = "2432MHz (Channel 5)";
ChLst_24G[5] = "2437MHz (Channel 6)";
ChLst_24G[6] = "2442MHz (Channel 7)";
ChLst_24G[7] = "2447MHz (Channel 8)";
ChLst_24G[8] = "2452MHz (Channel 9)";
ChLst_24G[9] = "2457MHz (Channel 10)";
ChLst_24G[10] = "2462MHz (Channel 11)";
ChLst_24G[11] = "2467MHz (Channel 12)";
ChLst_24G[12] = "2472MHz (Channel 13)";
ChLst_24G[13] = "2484MHz (Channel 14)";

ChLst_5G = new Array(33);
ChLst_5G[0] = "5180MHz (Channel 36)";
ChLst_5G[1] = "5200MHz (Channel 40)";
ChLst_5G[2] = "5220MHz (Channel 44)";
ChLst_5G[3] = "5240MHz (Channel 48)";
ChLst_5G[4] = "5260MHz (Channel 52)";
ChLst_5G[5] = "5280MHz (Channel 56)";
ChLst_5G[6] = "5300MHz (Channel 60)";
ChLst_5G[7] = "5320MHz (Channel 64)";
ChLst_5G[16] = "5500MHz (Channel 100)";
ChLst_5G[17] = "5520MHz (Channel 104)";
ChLst_5G[18] = "5540MHz (Channel 108)";
ChLst_5G[19] = "5560MHz (Channel 112)";
ChLst_5G[20] = "5580MHz (Channel 116)";
ChLst_5G[21] = "5600MHz (Channel 120)";
ChLst_5G[22] = "5620MHz (Channel 124)";
ChLst_5G[23] = "5640MHz (Channel 128)";
ChLst_5G[24] = "5660MHz (Channel 132)";
ChLst_5G[25] = "5680MHz (Channel 136)";
ChLst_5G[26] = "5700MHz (Channel 140)";
ChLst_5G[28] = "5745MHz (Channel 149)";
ChLst_5G[29] = "5765MHz (Channel 153)";
ChLst_5G[30] = "5785MHz (Channel 157)";
ChLst_5G[31] = "5805MHz (Channel 161)";
ChLst_5G[32] = "5825MHz (Channel 165)";

HT5GExtCh = new Array(22);
HT5GExtCh[0] = new Array(1, "5200MHz (Channel 40)"); //36's extension channel
HT5GExtCh[1] = new Array(0, "5180MHz (Channel 36)"); //40's extension channel
HT5GExtCh[2] = new Array(1, "5240MHz (Channel 48)"); //44's
HT5GExtCh[3] = new Array(0, "5220MHz (Channel 44)"); //48's
HT5GExtCh[4] = new Array(1, "5280MHz (Channel 56)"); //52's
HT5GExtCh[5] = new Array(0, "5260MHz (Channel 52)"); //56's
HT5GExtCh[6] = new Array(1, "5320MHz (Channel 64)"); //60's
HT5GExtCh[7] = new Array(0, "5300MHz (Channel 60)"); //64's
HT5GExtCh[8] = new Array(1, "5520MHz (Channel 104)"); //100's
HT5GExtCh[9] = new Array(0, "5500MHz (Channel 100)"); //104's
HT5GExtCh[10] = new Array(1, "5560MHz (Channel 112)"); //108's
HT5GExtCh[11] = new Array(0, "5540MHz (Channel 108)"); //112's
HT5GExtCh[12] = new Array(1, "5600MHz (Channel 120)"); //116's
HT5GExtCh[13] = new Array(0, "5580MHz (Channel 116)"); //120's
HT5GExtCh[14] = new Array(1, "5640MHz (Channel 128)"); //124's
HT5GExtCh[15] = new Array(0, "5620MHz (Channel 124)"); //128's
HT5GExtCh[16] = new Array(1, "5680MHz (Channel 136)"); //132's
HT5GExtCh[17] = new Array(0, "5660MHz (Channel 132)"); //136's
HT5GExtCh[18] = new Array(1, "5765MHz (Channel 153)"); //149's
HT5GExtCh[19] = new Array(0, "5745MHz (Channel 149)"); //153's
HT5GExtCh[20] = new Array(1, "5805MHz (Channel 161)"); //157's
HT5GExtCh[21] = new Array(0, "5785MHz (Channel 157)"); //161's


function StylDispOn()
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

function CreateExtChOpt(vChannel)
{
	var y = document.createElement('option');

	y.text = ChLst_24G[1*vChannel - 1];
//	y.value = 1*vChannel;
	y.value = 1;

	var x = document.getElementById("n_extcha");

	try
	{
		x.add(y,null); // standards compliant
	}
	catch(ex)
	{
		x.add(y); // IE only
	}
}

function InsExtChOpt()
{
	var wmode = document.wireless_basic.wirelessmode.options.selectedIndex;
	var OptLen; 
	var CurrCh;

	if ((1*wmode == 6) || (1*wmode == 3) || (1*wmode == 4) || (1*wmode == 7))
	{
		var x = document.getElementById("n_extcha");
		var length = document.wireless_basic.n_extcha.options.length;

		if (length > 1)
		{
			x.selectedIndex = 1;
			x.remove(x.selectedIndex);
		}

		if ((1*wmode == 6) || (1*wmode == 7))
		{
			CurrCh = document.wireless_basic.sz11aChannel.value;

			if ((1*CurrCh >= 36) && (1*CurrCh <= 64))
			{
				CurrCh = 1*CurrCh;
				CurrCh /= 4;
				CurrCh -= 9;

				x.options[0].text = HT5GExtCh[CurrCh][1];
				x.options[0].value = HT5GExtCh[CurrCh][0];
			}
			else if ((1*CurrCh >= 100) && (1*CurrCh <= 136))
			{
				CurrCh = 1*CurrCh;
				CurrCh /= 4;
				CurrCh -= 17;

				x.options[0].text = HT5GExtCh[CurrCh][1];
				x.options[0].value = HT5GExtCh[CurrCh][0];
			}
			else if ((1*CurrCh >= 149) && (1*CurrCh <= 161))
			{
				CurrCh = 1*CurrCh;
				CurrCh -= 1;
				CurrCh /= 4;
				CurrCh -= 19;

				x.options[0].text = HT5GExtCh[CurrCh][1];
				x.options[0].value = HT5GExtCh[CurrCh][0];
			}
			else
			{
				x.options[0].text = "Auto Select";
				x.options[0].value = 0;
			}
		}
		else if ((1*wmode == 3) || (1*wmode == 4))
		{
			CurrCh = document.wireless_basic.sz11gChannel.value;
			OptLen = document.wireless_basic.sz11gChannel.options.length;

			if ((CurrCh >=1) && (CurrCh <= 4))
			{
				x.options[0].text = ChLst_24G[1*CurrCh + 4 - 1];
				x.options[0].value = 1;
			}
			else if ((CurrCh >= 5) && (CurrCh <= 7))
			{
				x.options[0].text = ChLst_24G[1*CurrCh - 4 - 1];
				x.options[0].value = 0; //1*CurrCh - 4;
				CurrCh = 1*CurrCh;
				CurrCh += 4;
				CreateExtChOpt(CurrCh);
			}
			else if ((CurrCh >= 8) && (CurrCh <= 9))
			{
				x.options[0].text = ChLst_24G[1*CurrCh - 4 - 1];
				x.options[0].value = 0; //1*CurrCh - 4;

				if (OptLen >=14)
				{
					CurrCh = 1*CurrCh;
					CurrCh += 4;
					CreateExtChOpt(CurrCh);
				}
			}
			else if (CurrCh == 10)
			{
				x.options[0].text = ChLst_24G[1*CurrCh - 4 - 1];
				x.options[0].value = 0; //1*CurrCh - 4;

				if (OptLen > 14)
				{
					CurrCh = 1*CurrCh;
					CurrCh += 4;
					CreateExtChOpt(CurrCh);
				}
			}
			else if (CurrCh >= 11)
			{
				x.options[0].text = ChLst_24G[1*CurrCh - 4 - 1];
				x.options[0].value = 0; //1*CurrCh - 4;
			}
			else
			{
				x.options[0].text = "Auto Select";
				x.options[0].value = 0;
			}
		}
	}
}

function ChOnChange()
{
	if (document.wireless_basic.n_bandwidth[1].checked == true)
	{
		var w_mode = document.wireless_basic.wirelessmode.options.selectedIndex;

		if ((1*w_mode == 6) || (1*w_mode == 7))
		{
			if (document.wireless_basic.n_bandwidth[1].checked == true)
			{
				document.getElementById("extension_channel").style.visibility = "visible";
				document.getElementById("extension_channel").style.display = StylDispOn();
				document.wireless_basic.n_extcha.disabled = false;
			}

			if (document.wireless_basic.sz11aChannel.options.selectedIndex == 0)
			{
				document.getElementById("extension_channel").style.visibility = "hidden";
				document.getElementById("extension_channel").style.display = "none";
				document.wireless_basic.n_extcha.disabled = true;
			}
		}
		else if ((1*w_mode == 3) || (1*w_mode == 4))
		{
			if (document.wireless_basic.n_bandwidth[1].checked == true)
			{
				document.getElementById("extension_channel").style.visibility = "visible";
				document.getElementById("extension_channel").style.display = StylDispOn();
				document.wireless_basic.n_extcha.disabled = false;
			}

			if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
			{
				document.getElementById("extension_channel").style.visibility = "hidden";
				document.getElementById("extension_channel").style.display = "none";
				document.wireless_basic.n_extcha.disabled = true;
			}
		}
	}

	InsExtChOpt();
}

function ChBwOnClick()
{
	var w_mode = document.wireless_basic.wirelessmode.options.selectedIndex;

	if (document.wireless_basic.n_bandwidth[0].checked == true)
	{
		document.getElementById("extension_channel").style.visibility = "hidden";
		document.getElementById("extension_channel").style.display = "none";
		document.wireless_basic.n_extcha.disabled = true;
	}
	else
	{
		document.getElementById("extension_channel").style.visibility = "visible";
		document.getElementById("extension_channel").style.display = StylDispOn();
		document.wireless_basic.n_extcha.disabled = false;

		if ((1*w_mode == 6) || (1*w_mode == 7))
		{
			if (document.wireless_basic.sz11aChannel.options.selectedIndex == 0)
			{
				document.getElementById("extension_channel").style.visibility = "hidden";
				document.getElementById("extension_channel").style.display = "none";
				document.wireless_basic.n_extcha.disabled = true;
			}
		}
	}
}


function initTranslation()
{
	var e = document.getElementById("basicTitle");
	e.innerHTML = _("basic title");
	e = document.getElementById("basicIntroduction");
	e.innerHTML = _("basic introduction");

	e = document.getElementById("basicWirelessNet");
	e.innerHTML = _("basic wireless network");
	e = document.getElementById("aboutDriverVersion");
	e.innerHTML = _("about driver version");
	e = document.getElementById("basicRadioButton");
	e.innerHTML = _("basic radio button");
	e = document.getElementById("basicWiFiButton");
	e.innerHTML = _("basic wifi button");
	e = document.getElementById("basicNetMode");
	e.innerHTML = _("basic network mode");
	e = document.getElementById("basicSSID");
	e.innerHTML = _("basic ssid");
	e = document.getElementById("basicHSSID0");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID1");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID2");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID3");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID4");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID5");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID6");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID7");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID8");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID9");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID10");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID11");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID12");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID13");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID14");
	e.innerHTML = _("basic hssid");
	e = document.getElementById("basicHSSID15");

	e = document.getElementById("basicMSSID1");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID2");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID3");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID4");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID5");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID6");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID7");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID8");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID9");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID10");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID11");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID12");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID13");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID14");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicMSSID15");
	e.innerHTML = _("basic multiple ssid");
	e = document.getElementById("basicBroadcastSSIDEnable");
	e.innerHTML = _("wireless enable");
	e = document.getElementById("basicBroadcastSSIDDisable");
	e.innerHTML = _("wireless disable");
	e = document.getElementById("basicBroadcastSSID");
	e.innerHTML = _("basic broadcast ssid");
	e = document.getElementById("basicApIsolatedEnable");
	e.innerHTML = _("wireless enable");
	e = document.getElementById("basicApIsolatedDisable");
	e.innerHTML = _("wireless disable");
	e = document.getElementById("basicApIsolated");
	e.innerHTML = _("basic apisolated");
	e = document.getElementById("basicIsolatedSSID0");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID1");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID2");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID3");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID4");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID5");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID6");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID7");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID8");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID9");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID10");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID11");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID12");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID13");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID14");
	e.innerHTML = _("basic isolated");
	e = document.getElementById("basicIsolatedSSID15");
	e.innerHTML = _("basic isolated");

	e = document.getElementById("basicMBSSIDApIsolatedEnable");
	e.innerHTML = _("wireless enable");
	e = document.getElementById("basicMBSSIDApIsolatedDisable");
	e.innerHTML = _("wireless disable");
	e = document.getElementById("basicMBSSIDApIsolated");
	e.innerHTML = _("basic mbssidapisolated");
	e = document.getElementById("basicBSSID");
	e.innerHTML = _("basic bssid");
	e = document.getElementById("basicFreqA");
	e.innerHTML = _("basic frequency");
	e = document.getElementById("basicFreqAAuto");
	e.innerHTML = _("basic frequency auto");
	e = document.getElementById("basicFreqB");
	e.innerHTML = _("basic frequency");
	e = document.getElementById("basicFreqBAuto");
	e.innerHTML = _("basic frequency auto");
	e = document.getElementById("basicFreqG");
	e.innerHTML = _("basic frequency");
	e = document.getElementById("basicFreqGAuto");
	e.innerHTML = _("basic frequency auto");
	e = document.getElementById("basicRate");
	e.innerHTML = _("basic rate");

	e = document.getElementById("basicHTPhyMode");
	e.innerHTML = _("basic ht phy mode");
	e = document.getElementById("basicHTOPMode");
	e.innerHTML = _("basic ht op mode");
      	e = document.getElementById("basicHTMixed");
	e.innerHTML = _("basic ht op mixed");
    	e = document.getElementById("basicHTChannelBW");
	e.innerHTML = _("basic ht channel bandwidth");
    	e = document.getElementById("basicHTGI");
	e.innerHTML = _("basic ht guard interval");
      	e = document.getElementById("basicHTLongGI");
	e.innerHTML = _("wireless long");
      	e = document.getElementById("basicHTAutoGI");
	e.innerHTML = _("wireless auto");
    	e = document.getElementById("basicHTAutoMCS");
	e.innerHTML = _("wireless auto");
    	e = document.getElementById("basicHTRDG");
	e.innerHTML = _("basic ht rdg");
    	e = document.getElementById("basicHTRDGDisable");
	e.innerHTML = _("wireless disable");
    	e = document.getElementById("basicHTRDGEnable");
	e.innerHTML = _("wireless enable");
    	e = document.getElementById("basicHTExtChannel");
	e.innerHTML = _("basic ht extension channel");
	e = document.getElementById("basicHTSTBC");
	e.innerHTML = _("basic ht stbc");
	e = document.getElementById("basicHTSTBCDisable");
	e.innerHTML = _("wireless disable");
	e = document.getElementById("basicHTSTBCEnable");
	e.innerHTML = _("wireless enable");
    	e = document.getElementById("basicHTAMSDU");
	e.innerHTML = _("basic ht amsdu");
    	e = document.getElementById("basicHTAMSDUDisable");
	e.innerHTML = _("wireless disable");
    	e = document.getElementById("basicHTAMSDUEnable");
	e.innerHTML = _("wireless enable");
    	e = document.getElementById("basicHTAddBA");
	e.innerHTML = _("basic ht addba");
    	e = document.getElementById("basicHTAddBADisable");
	e.innerHTML = _("wireless disable");
    	e = document.getElementById("basicHTAddBAEnable");
	e.innerHTML = _("wireless enable");
    	e = document.getElementById("basicHTDelBA");
	e.innerHTML = _("basic ht delba");
    	e = document.getElementById("basicHTDelBADisable");
	e.innerHTML = _("wireless disable");
    	e = document.getElementById("basicHTDelBAEnable");
	e.innerHTML = _("wireless enable");

    	e = document.getElementById("basicOther");
	e.innerHTML = _("basic other");
    	e = document.getElementById("basicHTTxStream");
	e.innerHTML = _("basic ht txstream");
    	e = document.getElementById("basicHTRxStream");
	e.innerHTML = _("basic ht rxstream");

	e = document.getElementById("basicApply");
	e.value = _("wireless apply");
	e = document.getElementById("basicCancel");
	e.value = _("wireless cancel");
}

function initValue()
{
	var Ch11aIdx;
	var CurrChLen;
	var radio_off = '<% getCfgZero(1, "RadioOff"); %>';
	var wifi_off = '<% getCfgZero(1, "WiFiOff"); %>';
	var mssidb = "<% getMBSSIDBuilt(); %>";
	var BssidNum = 1*'<% getCfgGeneral(1, "BssidNum"); %>';
	var i = 0;

	initTranslation();

	if (CntyCd == '')
		CntyCd = 'NONE';

	document.getElementById("div_11a_channel").style.visibility = "hidden";
	document.getElementById("div_11a_channel").style.display = "none";
	document.wireless_basic.sz11aChannel.disabled = true;
	document.getElementById("div_11b_channel").style.visibility = "hidden";
	document.getElementById("div_11b_channel").style.display = "none";
	document.wireless_basic.sz11bChannel.disabled = true;
	document.getElementById("div_11g_channel").style.visibility = "hidden";
	document.getElementById("div_11g_channel").style.display = "none";
	document.wireless_basic.sz11gChannel.disabled = true;
	document.getElementById("div_11n").style.display = "none";
	document.wireless_basic.n_mode.disabled = true;
	document.wireless_basic.n_bandwidth.disabled = true;
	document.wireless_basic.n_rdg.disabled = true;
	document.wireless_basic.n_gi.disabled = true;
	document.wireless_basic.n_mcs.disabled = true;
	document.getElementById("div_2040_coexit").style.display = "none";
	document.wireless_basic.n_2040_coexit.disabled = true;

	PhyMode = 1*PhyMode;

	if ((PhyMode >= 8) || (PhyMode == 6))
	{
		if (window.ActiveXObject) // IE
			document.getElementById("div_11n").style.display = "block";
		else if (window.XMLHttpRequest)  // Mozilla, Safari,...
			document.getElementById("div_11n").style.display = "table";
		document.wireless_basic.n_mode.disabled = false;
		document.wireless_basic.n_bandwidth.disabled = false;
		document.wireless_basic.n_rdg.disabled = false;
		document.wireless_basic.n_gi.disabled = false;
		document.wireless_basic.n_mcs.disabled = false;
	}

	var Aband = "<% getRaxABand(); %>";
	if (Aband == "1")
	{
		document.wireless_basic.wirelessmode.options[5] = new Option("11a only", "2");
                document.wireless_basic.wirelessmode.options[6] = new Option("11a/n mixed mode", "8");
                document.wireless_basic.wirelessmode.options[7] = new Option("11n only(5G)", "11");
	}
	if ((PhyMode == 0) || (PhyMode == 4) || (PhyMode == 9) || (PhyMode == 6))
	{
		if (PhyMode == 0)
			document.wireless_basic.wirelessmode.options.selectedIndex = 0;
		else if (PhyMode == 4)
			document.wireless_basic.wirelessmode.options.selectedIndex = 2;
		else if (PhyMode == 9)
			document.wireless_basic.wirelessmode.options.selectedIndex = 3;
		else if (PhyMode == 6)
			document.wireless_basic.wirelessmode.options.selectedIndex = 4;

		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = StylDispOn();
		document.wireless_basic.sz11gChannel.disabled = false;
	}
	else if (PhyMode == 1)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 1;
		document.getElementById("div_11b_channel").style.visibility = "visible";
		document.getElementById("div_11b_channel").style.display = StylDispOn();
		document.wireless_basic.sz11bChannel.disabled = false;
	}
	else if ((PhyMode == 2) || (PhyMode == 8) || (PhyMode == 11))
	{
		if (PhyMode == 2)
			document.wireless_basic.wirelessmode.options.selectedIndex = 5;
		else if (PhyMode == 8)
			document.wireless_basic.wirelessmode.options.selectedIndex = 6;
		else if (PhyMode == 11)
			document.wireless_basic.wirelessmode.options.selectedIndex = 7;
		document.getElementById("div_11a_channel").style.visibility = "visible";
		document.getElementById("div_11a_channel").style.display = StylDispOn();
		document.wireless_basic.sz11aChannel.disabled = false;
	}

	for (i=1;i<16;i++)
	{
		var ssid = eval("document.wireless_basic.mssid_"+i+".disabled");
		var div = eval("document.getElementById(\"div_hssid"+i+"\").style");

		div.visibility = "hidden";
		div.display = "none";
		ssid = true;
		document.wireless_basic.hssid[i].disabled = true;
	}
	if (mssidb == "1")
	{
		var mssid16b = "<% get16MBSSIDBuilt(); %>";
		var num = 8;
		if (mssid16b == "1")
			num = 16;
		for (i=1;i<num;i++)
		{
			var ssid = eval("document.wireless_basic.mssid_"+i+".disabled");
			var div = eval("document.getElementById(\"div_hssid"+i+"\").style");

			div.visibility = "visible";
			div.display = StylDispOn();
			ssid = false;
			document.wireless_basic.hssid[i].disabled = false;
		}
	}

	if (HiddenSSID.indexOf("0") >= 0)
	{
		document.wireless_basic.broadcastssid[0].checked = true;
		var HiddenSSIDArray = HiddenSSID.split(";");
		for (i=0;i<BssidNum;i++)
		{
			if (HiddenSSIDArray[i] == "1")
				document.wireless_basic.hssid[i].checked = true;
			else
				document.wireless_basic.hssid[i].checked = false;
		}
	}
	else
	{
		document.wireless_basic.broadcastssid[1].checked = true;
		for (i=0;i<BssidNum;i++)
			document.wireless_basic.hssid[i].checked = true;
	}

	if (APIsolated.indexOf("1") >= 0)
	{
		document.wireless_basic.apisolated[0].checked = true;
		var APIsolatedArray = APIsolated.split(";");
		for (i=0;i<BssidNum;i++)
		{
			if (APIsolatedArray[i] == "1")
				document.wireless_basic.isolated_ssid[i].checked = true;
			else
				document.wireless_basic.isolated_ssid[i].checked = false;
		}
	}
	else
	{
		document.wireless_basic.apisolated[1].checked = true;
		for (i=0;i<BssidNum;i++)
			document.wireless_basic.isolated_ssid[i].checked = false;
	}

	if (1*ht_bw == 0)
	{
		document.wireless_basic.n_bandwidth[0].checked = true;
		document.getElementById("extension_channel").style.visibility = "hidden";
		document.getElementById("extension_channel").style.display = "none";
		document.wireless_basic.n_extcha.disabled = true;
	}
	else
	{
		document.wireless_basic.n_bandwidth[1].checked = true;
		document.getElementById("extension_channel").style.visibility = "visible";
		document.getElementById("extension_channel").style.display = StylDispOn();
		document.wireless_basic.n_extcha.disabled = false;
	}

	ChIdx = 1*ChIdx;

	if ((PhyMode == 0) || (PhyMode == 4) || (PhyMode == 6) || (PhyMode == 7) || (PhyMode == 9))
		{
		if ((CntyCd == 'US' || CntyCd == 'TW') && (ChIdx < 1 || ChIdx > 11))
			{
			document.wireless_basic.sz11gChannel.options.selectedIndex = 0;
			}
		else if ((CntyCd == 'FR' || CntyCd == 'IE' || CntyCd == 'HK') && (ChIdx < 1 || ChIdx > 13))
			{
			document.wireless_basic.sz11gChannel.options.selectedIndex = 0;
			}
		else if (CntyCd == 'JP' && (ChIdx < 1 || ChIdx > 14))
			{
			document.wireless_basic.sz11gChannel.options.selectedIndex = 0;
			}
			else
			{
			if (ChIdx < 1 || ChIdx > 14)
				document.wireless_basic.sz11gChannel.options.selectedIndex = 0;
			}
		}
	else if (PhyMode == 1)
			{
		if ((CntyCd == 'US' || CntyCd == 'TW') && (ChIdx < 1 || ChIdx > 11))
			{
			document.wireless_basic.sz11bChannel.options.selectedIndex = 0;
			}
		else if ((CntyCd == 'FR' || CntyCd == 'IE' || CntyCd == 'HK') && (ChIdx < 1 || ChIdx > 13))
			{
			document.wireless_basic.sz11bChannel.options.selectedIndex = 0;
		}
		else if (CntyCd == 'JP' && (ChIdx < 1 || ChIdx > 14))
			{
			document.wireless_basic.sz11bChannel.options.selectedIndex = 0;
			}
			else
			{
			if (ChIdx < 1 || ChIdx > 14)
				document.wireless_basic.sz11bChannel.options.selectedIndex = 0;
			}
		}
	else if ((PhyMode == 2) || (PhyMode == 8) || (PhyMode == 11))
		{
		if (CntyCd == 'HK' && (ChIdx < 36 || (ChIdx > 64 && ChIdx < 149) || ChIdx > 165))
			{
			document.wireless_basic.sz11aChannel.options.selectedIndex = 0;
			}
		else if (CntyCd == 'IE' && (ChIdx < 36 || (ChIdx > 64 && ChIdx < 100) || ChIdx > 140))
			{
			document.wireless_basic.sz11aChannel.options.selectedIndex = 0;
			}
		else if (CntyCd == 'FR' && (ChIdx < 36 || ChIdx > 64))
			{
			document.wireless_basic.sz11aChannel.options.selectedIndex = 0;
		}
		else if (CntyCd == 'JP' && (ChIdx < 36 || ChIdx > 48))
		{
			document.wireless_basic.sz11aChannel.options.selectedIndex = 0;
			}
		else if (CntyCd == 'US' && (ChIdx < 36 || (ChIdx > 64 && ChIdx < 100) || (ChIdx > 140 && ChIdx < 149) || ChIdx > 165))
			{
			document.wireless_basic.sz11aChannel.options.selectedIndex = 0;
			}
		else if (CntyCd == 'TW' && (ChIdx < 52 || ChIdx > 64))
		{
			document.wireless_basic.sz11aChannel.options.selectedIndex = 0;
		}
		else
		{
			if (ChIdx < 36 || (ChIdx > 64 && ChIdx < 100) || (ChIdx > 140 && ChIdx < 149) || ChIdx > 165)
				document.wireless_basic.sz11aChannel.options.selectedIndex = 0;
		}
	}

	//ABG Rate
	if ((PhyMode == 0) || (PhyMode == 2) || (PhyMode == 4))
	{
		document.wireless_basic.abg_rate.options.length = 0;
		document.wireless_basic.abg_rate.options[0] = new Option("Auto", "0");
		document.wireless_basic.abg_rate.options[1] = new Option("1 Mbps", "1");
		document.wireless_basic.abg_rate.options[2] = new Option("2 Mbps", "2");
		document.wireless_basic.abg_rate.options[3] = new Option("5.5 Mbps", "5");
		document.wireless_basic.abg_rate.options[4] = new Option("6 Mbps", "6");
		document.wireless_basic.abg_rate.options[5] = new Option("9 Mbps", "9");
		document.wireless_basic.abg_rate.options[6] = new Option("11 Mbps", "11");
		document.wireless_basic.abg_rate.options[7] = new Option("12 Mbps", "12");
		document.wireless_basic.abg_rate.options[8] = new Option("18 Mbps", "18");
		document.wireless_basic.abg_rate.options[9] = new Option("24 Mbps", "24");
		document.wireless_basic.abg_rate.options[10] = new Option("36 Mbps", "36");
		document.wireless_basic.abg_rate.options[11] = new Option("48 Mbps", "48");
		document.wireless_basic.abg_rate.options[12] = new Option("54 Mbps", "54");
		if (fxtxmode == "CCK" || fxtxmode == "cck") {
			if (ht_mcs.indexOf("33") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 0;
			else if (ht_mcs.indexOf("0") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 1;
			else if (ht_mcs.indexOf("1") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 2;
			else if (ht_mcs.indexOf("2") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 3;
			else if (ht_mcs.indexOf("3") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 6;
		}
		else {
			if (ht_mcs.indexOf("33") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 0;
			else if (ht_mcs.indexOf("0") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 4;
			else if (ht_mcs.indexOf("1") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 5;
			else if (ht_mcs.indexOf("2") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 7;
			else if (ht_mcs.indexOf("3") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 8;
			else if (ht_mcs.indexOf("4") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 9;
			else if (ht_mcs.indexOf("5") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 10;
			else if (ht_mcs.indexOf("6") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 11;
			else if (ht_mcs.indexOf("7") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 12;
		}
	}
	else if (PhyMode == 1)
	{
		document.wireless_basic.abg_rate.options.length = 0;
		document.wireless_basic.abg_rate.options[0] = new Option("Auto", "0");
		document.wireless_basic.abg_rate.options[1] = new Option("1 Mbps", "1");
		document.wireless_basic.abg_rate.options[2] = new Option("2 Mbps", "2");
		document.wireless_basic.abg_rate.options[3] = new Option("5.5 Mbps", "5");
		document.wireless_basic.abg_rate.options[4] = new Option("11 Mbps", "11");
		if (ht_mcs.indexOf("33") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 0;
		else if (ht_mcs.indexOf("0") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 1;
		else if (ht_mcs.indexOf("1") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 2;
		else if (ht_mcs.indexOf("2") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 3;
		else if (ht_mcs.indexOf("3") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 4;
	}
	else
	{
		document.getElementById("div_abg_rate").style.visibility = "hidden";
		document.getElementById("div_abg_rate").style.display = "none";
		document.wireless_basic.abg_rate.disabled = true;
	}

	document.getElementById("div_mbssidapisolated").style.visibility = "hidden";
	document.getElementById("div_mbssidapisolated").style.display = "none";
	document.wireless_basic.mbssidapisolated.disabled = true;
	if (mssidb == "1")
	{
		document.getElementById("div_mbssidapisolated").style.visibility = "visible";
		document.getElementById("div_mbssidapisolated").style.display = StylDispOn();
		document.wireless_basic.mbssidapisolated.disabled = false;
		if (mbssidapisolated == "1")
			document.wireless_basic.mbssidapisolated[0].checked = true;
		else
			document.wireless_basic.mbssidapisolated[1].checked = true;
	}

	InsExtChOpt();

	if (1*ht_stbc == 0)
	{
		document.wireless_basic.n_stbc[0].checked = true;
	}
	else
	{
		document.wireless_basic.n_stbc[1].checked = true;
	}

	if (1*ht_mode == 0)
	{
		document.wireless_basic.n_mode[0].checked = true;
	}
	else if (1*ht_mode == 1)
	{
		document.wireless_basic.n_mode[1].checked = true;
	}

	else if (1*ht_mode == 2)
	{
		document.wireless_basic.n_mode[2].checked = true;
	}

	if (1*ht_gi == 0)
	{
		document.wireless_basic.n_gi[0].checked = true;
	}
	else if (1*ht_gi == 1)
	{
		document.wireless_basic.n_gi[1].checked = true;
	}
	else if (1*ht_gi == 2)
	{
		document.wireless_basic.n_gi[2].checked = true;
	}

	if (1*txrxStream == 3) {
		for (i = 16; i < 24; i++) {
			document.wireless_basic.n_mcs.options[i] = new Option(i, i);
		}
	}
	var mcs_length = document.wireless_basic.n_mcs.options.length;
	if (txrxStream == "3") {
		document.wireless_basic.n_mcs.options[mcs_length] = new Option("32", "32");
		mcs_length++;
		document.wireless_basic.n_mcs.options[mcs_length] = new Option("Auto", "33");
		mcs_length++;
	}

	var ht_mcs_nm = 1*ht_mcs.split(";", 1);
        if (ht_mcs_nm <= 23)
                document.wireless_basic.n_mcs.options.selectedIndex = ht_mcs_nm;
        else if (ht_mcs_nm == 32)
		document.wireless_basic.n_mcs.options.selectedIndex = mcs_length-2;
        else if (ht_mcs_nm == 33)
		document.wireless_basic.n_mcs.options.selectedIndex = mcs_length-1;

	if (1*ht_rdg == 0)
		document.wireless_basic.n_rdg[0].checked = true;
	else
		document.wireless_basic.n_rdg[1].checked = true;

	var OptLen = document.wireless_basic.n_extcha.options.length;

	if (1*ht_extcha == 0)
	{
		if (OptLen > 1)
			document.wireless_basic.n_extcha.options.selectedIndex = 0;
	}
	else if (1*ht_extcha == 1)
	{
		if (OptLen > 1)
			document.wireless_basic.n_extcha.options.selectedIndex = 1;
	}
	else
	{
		document.wireless_basic.n_extcha.options.selectedIndex = 0;
	}

	if ((1*PhyMode == 8) || (1*PhyMode == 11))
	{
		if (document.wireless_basic.sz11aChannel.options.selectedIndex == 0)
		{
			document.getElementById("extension_channel").style.visibility = "hidden";
			document.getElementById("extension_channel").style.display = "none";
			document.wireless_basic.n_extcha.disabled = true;
		}
	}
	else if ((1*PhyMode == 9) || (1*PhyMode == 6))
	{
		if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
		{
			document.getElementById("extension_channel").style.visibility = "hidden";
			document.getElementById("extension_channel").style.display = "none";
			document.wireless_basic.n_extcha.disabled = true;
		}
	}

	if (1*ht_amsdu == 0)
		document.wireless_basic.n_amsdu[0].checked = true;
	else
		document.wireless_basic.n_amsdu[1].checked = true;

	if (1*ht_autoba == 0)
		document.wireless_basic.n_autoba[0].checked = true;
	else
		document.wireless_basic.n_autoba[1].checked = true;

	if (1*ht_badecline == 0)
		document.wireless_basic.n_badecline[0].checked = true;
	else
		document.wireless_basic.n_badecline[1].checked = true;

	if (1*ht_disallow_tkip == 1)
		document.wireless_basic.n_disallow_tkip[1].checked = true;
	else
		document.wireless_basic.n_disallow_tkip[0].checked = true;

	if (1*draft3b == 1)
	{
		document.getElementById("div_2040_coexit").style.display = StylDispOn();
		document.wireless_basic.n_2040_coexit.disabled = false;
		if (1*ht_2040_coexit == 0)
			document.wireless_basic.n_2040_coexit[0].checked = true;
		else
			document.wireless_basic.n_2040_coexit[1].checked = true;
	}

	if (1*mesh_include == 1 && 1*apcli_include == 1)
	{
		document.wireless_basic.mssid_6.disabled = true;
		document.wireless_basic.mssid_7.disabled = true;
	}
	else if (1*mesh_include == 1 || 1*apcli_include == 1)
		document.wireless_basic.mssid_7.disabled = true;

	if (txrxStream == "2")
	{
		document.getElementById("div_HtTx2Stream").style.visibility = "visible";
		document.getElementById("div_HtTx2Stream").style.display = StylDispOn();
		document.getElementById("div_HtRx2Stream").style.visibility = "visible";
		document.getElementById("div_HtRx2Stream").style.display = StylDispOn();
	}
	else if (txrxStream == "3")
	{
		document.wireless_basic.rx_stream.options[2] = new Option("3", "3");
		document.wireless_basic.tx_stream.options[2] = new Option("3", "3");
	}
	else
	{
		document.getElementById("div_HtTx2Stream").style.visibility = "hidden";
		document.getElementById("div_HtTx2Stream").style.display = "none";
		tx_stream_idx = 1;
		document.getElementById("div_HtRx2Stream").style.visibility = "hidden";
		document.getElementById("div_HtRx2Stream").style.display = "none";
		rx_stream_idx = 1;
	}
	document.wireless_basic.rx_stream.options.selectedIndex = rx_stream_idx - 1;
	document.wireless_basic.tx_stream.options.selectedIndex = tx_stream_idx - 1;

	if (1*radio_off == 1)
		document.wireless_basic.radioButton.value = "RADIO ON";
	else
		document.wireless_basic.radioButton.value = "RADIO OFF";
	if (1*wifi_off == 1)
		document.wireless_basic.WiFiButton.value = "WiFi ON";
	else
		document.wireless_basic.WiFiButton.value = "WiFi OFF";
}

function wirelessModeChange()
{
	var wmode;
   
	document.getElementById("div_11a_channel").style.visibility = "hidden";
	document.getElementById("div_11a_channel").style.display = "none";
	document.wireless_basic.sz11aChannel.disabled = true;
	document.getElementById("div_11b_channel").style.visibility = "hidden";
	document.getElementById("div_11b_channel").style.display = "none";
	document.wireless_basic.sz11bChannel.disabled = true;
	document.getElementById("div_11g_channel").style.visibility = "hidden";
	document.getElementById("div_11g_channel").style.display = "none";
	document.wireless_basic.sz11gChannel.disabled = true;
	document.getElementById("div_abg_rate").style.visibility = "hidden";
	document.getElementById("div_abg_rate").style.display = "none";
	document.wireless_basic.abg_rate.disabled = true;
	document.getElementById("div_11n").style.display = "none";
	document.wireless_basic.n_mode.disabled = true;
	document.wireless_basic.n_bandwidth.disabled = true;
	document.wireless_basic.n_rdg.disabled = true;
	document.wireless_basic.n_gi.disabled = true;
	document.wireless_basic.n_mcs.disabled = true;

	wmode = document.wireless_basic.wirelessmode.options.selectedIndex;

	wmode = 1*wmode;
	if (wmode == 0)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 0;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = StylDispOn();
		document.wireless_basic.sz11gChannel.disabled = false;
	}
	else if (wmode == 1)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 1;
		document.getElementById("div_11b_channel").style.visibility = "visible";
		document.getElementById("div_11b_channel").style.display = StylDispOn();
		document.wireless_basic.sz11bChannel.disabled = false;
	}
	else if (wmode == 2)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 2;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = StylDispOn();
		document.wireless_basic.sz11gChannel.disabled = false;
	}
	else if (wmode == 5)
	{
		document.wireless_basic.wirelessmode.options.selectedIndex = 5;
		document.getElementById("div_11a_channel").style.visibility = "visible";
		document.getElementById("div_11a_channel").style.display = StylDispOn();
		document.wireless_basic.sz11aChannel.disabled = false;

	}
	else if ((wmode == 6) || (wmode == 7))
	{
		if (wmode == 7)
			document.wireless_basic.wirelessmode.options.selectedIndex = 7;
		else
			document.wireless_basic.wirelessmode.options.selectedIndex = 6;
		document.getElementById("div_11a_channel").style.visibility = "visible";
		document.getElementById("div_11a_channel").style.display = StylDispOn();
		document.wireless_basic.sz11aChannel.disabled = false;
		if (window.ActiveXObject) // IE
			document.getElementById("div_11n").style.display = "block";
		else if (window.XMLHttpRequest)  // Mozilla, Safari,...
			document.getElementById("div_11n").style.display = "table";
		document.wireless_basic.n_mode.disabled = false;
		document.wireless_basic.n_bandwidth.disabled = false;
		document.wireless_basic.n_rdg.disabled = false;
		document.wireless_basic.n_gi.disabled = false;
		document.wireless_basic.n_mcs.disabled = false;

		if (document.wireless_basic.sz11aChannel.options.selectedIndex == 0)
		{
			document.getElementById("extension_channel").style.visibility = "hidden";
			document.getElementById("extension_channel").style.display = "none";
			document.wireless_basic.n_extcha.disabled = true;
		}

		InsExtChOpt();
	}
	else if ((wmode == 3) || (wmode == 4))
	{
		if (wmode == 4)
			document.wireless_basic.wirelessmode.options.selectedIndex = 4;
		else
			document.wireless_basic.wirelessmode.options.selectedIndex = 3;
		document.getElementById("div_11g_channel").style.visibility = "visible";
		document.getElementById("div_11g_channel").style.display = StylDispOn();
		document.wireless_basic.sz11gChannel.disabled = false;
		if (window.ActiveXObject) // IE
			document.getElementById("div_11n").style.display = "block";
		else if (window.XMLHttpRequest)  // Mozilla, Safari,...
			document.getElementById("div_11n").style.display = "table";
		document.wireless_basic.n_mode.disabled = false;
		document.wireless_basic.n_bandwidth.disabled = false;
		document.wireless_basic.n_rdg.disabled = false;
		document.wireless_basic.n_gi.disabled = false;
		document.wireless_basic.n_mcs.disabled = false;

		if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
		{
			document.getElementById("extension_channel").style.visibility = "hidden";
			document.getElementById("extension_channel").style.display = "none";
			document.wireless_basic.n_extcha.disabled = true;
		}

		InsExtChOpt();
	}

	//ABG Rate
	if ((wmode == 0) || (wmode == 2) || (wmode == 5))
	{
		document.wireless_basic.abg_rate.options.length = 0;
		document.wireless_basic.abg_rate.options[0] = new Option("Auto", "0");
		document.wireless_basic.abg_rate.options[1] = new Option("1 Mbps", "1");
		document.wireless_basic.abg_rate.options[2] = new Option("2 Mbps", "2");
		document.wireless_basic.abg_rate.options[3] = new Option("5.5 Mbps", "5");
		document.wireless_basic.abg_rate.options[4] = new Option("6 Mbps", "6");
		document.wireless_basic.abg_rate.options[5] = new Option("9 Mbps", "9");
		document.wireless_basic.abg_rate.options[6] = new Option("11 Mbps", "11");
		document.wireless_basic.abg_rate.options[7] = new Option("12 Mbps", "12");
		document.wireless_basic.abg_rate.options[8] = new Option("18 Mbps", "18");
		document.wireless_basic.abg_rate.options[9] = new Option("24 Mbps", "24");
		document.wireless_basic.abg_rate.options[10] = new Option("36 Mbps", "36");
		document.wireless_basic.abg_rate.options[11] = new Option("48 Mbps", "48");
		document.wireless_basic.abg_rate.options[12] = new Option("54 Mbps", "54");
		if (fxtxmode == "CCK" || fxtxmode == "cck") {
			if (ht_mcs.indexOf("33") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 0;
			else if (ht_mcs.indexOf("0") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 1;
			else if (ht_mcs.indexOf("1") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 2;
			else if (ht_mcs.indexOf("2") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 3;
			else if (ht_mcs.indexOf("3") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 6;
		}
		else {
			if (ht_mcs.indexOf("33") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 0;
			else if (ht_mcs.indexOf("0") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 4;
			else if (ht_mcs.indexOf("1") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 5;
			else if (ht_mcs.indexOf("2") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 7;
			else if (ht_mcs.indexOf("3") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 8;
			else if (ht_mcs.indexOf("4") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 9;
			else if (ht_mcs.indexOf("5") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 10;
			else if (ht_mcs.indexOf("6") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 11;
			else if (ht_mcs.indexOf("7") == 0)
				document.wireless_basic.abg_rate.options.selectedIndex = 12;
		}

		document.getElementById("div_abg_rate").style.visibility = "visible";
		document.getElementById("div_abg_rate").style.display = StylDispOn();
		document.wireless_basic.abg_rate.disabled = false;
	}
	else if (wmode == 1)
	{
		document.wireless_basic.abg_rate.options.length = 0;
		document.wireless_basic.abg_rate.options[0] = new Option("Auto", "0");
		document.wireless_basic.abg_rate.options[1] = new Option("1 Mbps", "1");
		document.wireless_basic.abg_rate.options[2] = new Option("2 Mbps", "2");
		document.wireless_basic.abg_rate.options[3] = new Option("5.5 Mbps", "5");
		document.wireless_basic.abg_rate.options[4] = new Option("11 Mbps", "11");
		if (ht_mcs.indexOf("33") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 0;
		else if (ht_mcs.indexOf("0") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 1;
		else if (ht_mcs.indexOf("1") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 2;
		else if (ht_mcs.indexOf("2") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 3;
		else if (ht_mcs.indexOf("3") == 0)
			document.wireless_basic.abg_rate.options.selectedIndex = 4;

		document.getElementById("div_abg_rate").style.visibility = "visible";
		document.getElementById("div_abg_rate").style.display = StylDispOn();
		document.wireless_basic.abg_rate.disabled = false;
	}
}

function switch_hidden_ssid()
{
	var BssidNum = 1*'<% getCfgGeneral(1, "BssidNum"); %>';

	if (document.wireless_basic.broadcastssid[0].checked == true)
		for (i=0;i<BssidNum;i++)
			document.wireless_basic.hssid[i].checked = false;
	else
		for (i=0;i<BssidNum;i++)
			document.wireless_basic.hssid[i].checked = true;
}

function switch_isolated_ssid()
{
	var BssidNum = 1*'<% getCfgGeneral(1, "BssidNum"); %>';

	if (document.wireless_basic.apisolated[0].checked == true)
		for (i=0;i<BssidNum;i++)
			document.wireless_basic.isolated_ssid[i].checked = true;
	else
		for (i=0;i<BssidNum;i++)
			document.wireless_basic.isolated_ssid[i].checked = false;
}

function CheckValue()
{
	var wireless_mode;
	var submit_ssid_num;
	var Ch11aIdx;

	if (document.wireless_basic.mssid_0.value == "")
	{
		alert("Please enter SSID!");
		document.wireless_basic.mssid_0.focus();
		document.wireless_basic.mssid_0.select();
		return false;
	}

	var wpsenable = "<% getCfgZero(1, "WscModeOption"); %>";
	if (wpsenable != "0" && document.wireless_basic.hssid[0].checked == true)
		alert("WPS feature is going to turn off because of disabled broadcasting of SSID");

	submit_ssid_num = 1;

	for (i = 1; i < max_bssid_num; i++)
	{
		if (eval("document.wireless_basic.mssid_"+i).value != "")
		{
			if (i > 5)
			{
				if (1*mesh_include == 1 && 1*apcli_include == 1)
					break;
				else if (i == (max_bssid_num - 1))
					if (1*mesh_include == 1 || 1*apcli_include == 1)
						break;
			}
			submit_ssid_num++;
		}
	}

	document.wireless_basic.bssid_num.value = submit_ssid_num;
	return true;
}

function RadioStatusChange(rs)
{
	if (rs == 1) {
		document.wireless_basic.radioButton.value = "RADIO OFF";
		document.wireless_basic.radiohiddenButton.value = 0;
	}
	else {
		document.wireless_basic.radioButton.value = "RADIO ON";
		document.wireless_basic.radiohiddenButton.value = 1;
	}
}

function WiFiStatusChange(rs)
{
	if (rs == 1) {
		document.wireless_basic.WiFiButton.value = "WiFi OFF";
		document.wireless_basic.wifihiddenButton.value = 0;
	}
	else {
		document.wireless_basic.WiFiButton.value = "WiFi ON";
		document.wireless_basic.wifihiddenButton.value = 1;
	}
}
</script>
</head>


<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="basicTitle">Basic Wireless Settings </h1>
<p id="basicIntroduction"> You could configure the minimum number of Wireless settings for communication, such as Network Name (SSID) and Channel. The Access Point can be set simply with only the minimum setting items. </p>
<hr />

<form method=post name=wireless_basic action="/goform/wirelessBasic" onSubmit="return CheckValue()">
<table width="540" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
  <tr> 
    <td class="title" colspan="2" id="basicWirelessNet">Wireless Network</td>
  </tr>
  <tr>
    <td class="head" id="aboutDriverVersion">Driver Version</td>
    <td><% getAPDriverVer("ra0"); %></td>
  </tr>
  <tr> 
    <td class="head" id="basicRadioButton">Radio On/Off</td>
    <td>
      <input type="button" name="radioButton" style="{width:120px;}" value="RADIO ON"
      onClick="if (this.value.indexOf('OFF') >= 0) RadioStatusChange(1); else RadioStatusChange(0); document.wireless_basic.submit();"> &nbsp; &nbsp;
      <input type=hidden name=radiohiddenButton value="2">
    </td>
  </tr>
  <tr> 
    <td class="head" id="basicWiFiButton">WiFi UP/DOWN</td>
    <td>
      <input type="button" name="WiFiButton" style="{width:120px;}" value="WiFi ON"
      onClick="if (this.value.indexOf('OFF') >= 0) WiFiStatusChange(1); else WiFiStatusChange(0); document.wireless_basic.submit();"> &nbsp; &nbsp;
      <input type=hidden name=wifihiddenButton value="2">
    </td>
  </tr>
  <tr> 
    <td class="head" id="basicNetMode">Network Mode</td>
    <td>
      <select name="wirelessmode" id="wirelessmode" size="1" onChange="wirelessModeChange()">
        <option value=0>11b/g mixed mode</option>
        <option value=1>11b only</option>
        <option value=4>11g only</option>
        <option value=9>11b/g/n mixed mode</option>
	<option value=6>11n only(2.4G)</option>
      </select>
    </td>
  </tr>
  <input type="hidden" name="bssid_num" value="1">
  <tr> 
    <td class="head" id="basicSSID">Network Name(SSID)</td>
    <td>
      <input type=text name=mssid_0 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID1"); %>">
      &nbsp;&nbsp;<font id="basicHSSID0">Hidden</font>
      <input type=checkbox name=hssid value="0">
      &nbsp;&nbsp;<font id="basicIsolatedSSID0">Isolated</font>
      <input type=checkbox name=isolated_ssid value="0">
    </td>
  </tr>
  <tr id="div_hssid1"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID1">Multiple SSID</font>1</td>
    <td>
      <input type=text name=mssid_1 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID2"); %>">
      &nbsp;&nbsp;<font id="basicHSSID1">Hidden</font>
      <input type=checkbox name=hssid value="1">
      &nbsp;&nbsp;<font id="basicIsolatedSSID1">Isolated</font>
      <input type=checkbox name=isolated_ssid value="1">
    </td>
  </tr>
  <tr id="div_hssid2"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID2">Multiple SSID</font>2</td>
    <td>
      <input type=text name=mssid_2 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID3"); %>">
      &nbsp;&nbsp;<font id="basicHSSID2">Hidden</font>
      <input type=checkbox name=hssid value="2">
      &nbsp;&nbsp;<font id="basicIsolatedSSID2">Isolated</font>
      <input type=checkbox name=isolated_ssid value="2">
    </td>
  </tr>
  <tr id="div_hssid3"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID3">Multiple SSID</font>3</td>
    <td><input type=text name=mssid_3 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID4"); %>">
      &nbsp;&nbsp;<font id="basicHSSID3">Hidden</font>
      <input type=checkbox name=hssid value="3">
      &nbsp;&nbsp;<font id="basicIsolatedSSID3">Isolated</font>
      <input type=checkbox name=isolated_ssid value="3">
    </td>
  </tr>
  <tr id="div_hssid4"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID4">Multiple SSID</font>4</td>
    <td><input type=text name=mssid_4 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID5"); %>">
      &nbsp;&nbsp;<font id="basicHSSID4">Hidden</font>
      <input type=checkbox name=hssid value="4">
      &nbsp;&nbsp;<font id="basicIsolatedSSID4">Isolated</font>
      <input type=checkbox name=isolated_ssid value="4">
    </td>
  </tr>
  <tr id="div_hssid5"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID5">Multiple SSID</font>5</td>
    <td><input type=text name=mssid_5 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID6"); %>">
      &nbsp;&nbsp;<font id="basicHSSID5">Hidden</font>
      <input type=checkbox name=hssid value="5">
      &nbsp;&nbsp;<font id="basicIsolatedSSID5">Isolated</font>
      <input type=checkbox name=isolated_ssid value="5">
    </td>
  </tr>
  <tr id="div_hssid6"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID6">Multiple SSID</font>6</td>
    <td><input type=text name=mssid_6 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID7"); %>">
      &nbsp;&nbsp;<font id="basicHSSID6">Hidden</font>
      <input type=checkbox name=hssid value="6">
      &nbsp;&nbsp;<font id="basicIsolatedSSID6">Isolated</font>
      <input type=checkbox name=isolated_ssid value="6">
    </td>
  </tr>
  <tr id="div_hssid7"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID7">Multiple SSID</font>7</td>
    <td><input type=text name=mssid_7 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID8"); %>">
      &nbsp;&nbsp;<font id="basicHSSID7">Hidden</font>
      <input type=checkbox name=hssid value="7">
      &nbsp;&nbsp;<font id="basicIsolatedSSID7">Isolated</font>
      <input type=checkbox name=isolated_ssid value="7">
    </td>
  </tr>
  <tr id="div_hssid8"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID8">Multiple SSID</font>8</td>
    <td><input type=text name=mssid_8 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID9"); %>">
      &nbsp;&nbsp;<font id="basicHSSID8">Hidden</font>
      <input type=checkbox name=hssid value="8">
      &nbsp;&nbsp;<font id="basicIsolatedSSID8">Isolated</font>
      <input type=checkbox name=isolated_ssid value="8">
    </td>
  </tr>
  <tr id="div_hssid9"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID9">Multiple SSID</font>9</td>
    <td><input type=text name=mssid_9 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID10"); %>">
      &nbsp;&nbsp;<font id="basicHSSID9">Hidden</font>
      <input type=checkbox name=hssid value="9">
      &nbsp;&nbsp;<font id="basicIsolatedSSID9">Isolated</font>
      <input type=checkbox name=isolated_ssid value="9">
    </td>
  </tr>
  <tr id="div_hssid10"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID10">Multiple SSID</font>10</td>
    <td><input type=text name=mssid_10 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID11"); %>">
      &nbsp;&nbsp;<font id="basicHSSID10">Hidden</font>
      <input type=checkbox name=hssid value="10">
      &nbsp;&nbsp;<font id="basicIsolatedSSID10">Isolated</font>
      <input type=checkbox name=isolated_ssid value="10">
    </td>
  </tr>
  <tr id="div_hssid11"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID11">Multiple SSID</font>11</td>
    <td><input type=text name=mssid_11 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID12"); %>">
      &nbsp;&nbsp;<font id="basicHSSID11">Hidden</font>
      <input type=checkbox name=hssid value="11">
      &nbsp;&nbsp;<font id="basicIsolatedSSID11">Isolated</font>
      <input type=checkbox name=isolated_ssid value="11">
    </td>
  </tr>
  <tr id="div_hssid12"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID12">Multiple SSID</font>12</td>
    <td><input type=text name=mssid_12 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID13"); %>">
      &nbsp;&nbsp;<font id="basicHSSID12">Hidden</font>
      <input type=checkbox name=hssid value="12">
      &nbsp;&nbsp;<font id="basicIsolatedSSID12">Isolated</font>
      <input type=checkbox name=isolated_ssid value="12">
    </td>
  </tr>
  <tr id="div_hssid13"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID13">Multiple SSID</font>13</td>
    <td><input type=text name=mssid_13 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID14"); %>">
      &nbsp;&nbsp;<font id="basicHSSID13">Hidden</font>
      <input type=checkbox name=hssid value="13">
      &nbsp;&nbsp;<font id="basicIsolatedSSID13">Isolated</font>
      <input type=checkbox name=isolated_ssid value="13">
    </td>
  </tr>
  <tr id="div_hssid14"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID14">Multiple SSID</font>14</td>
    <td><input type=text name=mssid_14 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID15"); %>">
      &nbsp;&nbsp;<font id="basicHSSID14">Hidden</font>
      <input type=checkbox name=hssid value="14">
      &nbsp;&nbsp;<font id="basicIsolatedSSID14">Isolated</font>
      <input type=checkbox name=isolated_ssid value="14">
    </td>
  </tr>
  <tr id="div_hssid15"> 
    <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID15">Multiple SSID</font>15</td>
    <td><input type=text name=mssid_15 size=20 maxlength=32 value="<% getCfgToHTML(1, "SSID16"); %>">
      &nbsp;&nbsp;<font id="basicHSSID15">Hidden</font>
      <input type=checkbox name=hssid value="15">
      &nbsp;&nbsp;<font id="basicIsolatedSSID15">Isolated</font>
      <input type=checkbox name=isolated_ssid value="15">
    </td>
  </tr>
  <tr> 
    <td class="head" id="basicBroadcastSSID">Broadcast Network Name (SSID)</td>
    <td>
      <input type=radio name=broadcastssid value="1" checked onClick="switch_hidden_ssid()"><font id="basicBroadcastSSIDEnable">Enable&nbsp;</font>
      <input type=radio name=broadcastssid value="0" onClick="switch_hidden_ssid()"><font id="basicBroadcastSSIDDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="basicApIsolated">AP Isolation</td>
    <td>
      <input type=radio name=apisolated value="1" onClick="switch_isolated_ssid()"><font id="basicApIsolatedEnable">Enable&nbsp;</font>
      <input type=radio name=apisolated value="0" checked onClick="switch_isolated_ssid()"><font id="basicApIsolatedDisable">Disable</font>
    </td>
  </tr>
  <tr id="div_mbssidapisolated"> 
    <td class="head" id="basicMBSSIDApIsolated">MBSSID AP Isolation</td>
    <td>
      <input type=radio name=mbssidapisolated value="1"><font id="basicMBSSIDApIsolatedEnable">Enable&nbsp;</font>
      <input type=radio name=mbssidapisolated value="0" checked><font id="basicMBSSIDApIsolatedDisable">Disable</font>
    </td>
  </tr>
  <tr> 
    <td class="head" id="basicBSSID">BSSID</td>
    <td>&nbsp;&nbsp;<% getWlanCurrentMac(); %></td>
  </tr>
  <tr id="div_11a_channel" name="div_11a_channel">
    <td class="head"><font id="basicFreqA">Frequency (Channel)</font></td>
    <td>
      <select id="sz11aChannel" name="sz11aChannel" size="1" onChange="ChOnChange()">
	<option value=0 id="basicFreqAAuto">AutoSelect</option>
	<% getWlan11aChannels(); %>
      </select>
    </td>
  </tr>
  <tr id="div_11b_channel" name="div_11b_channel">
    <td class="head"><font id="basicFreqB">Frequency (Channel)</font></td>
    <td>
      <select id="sz11bChannel" name="sz11bChannel" size="1" onChange="ChOnChange()">
	<option value=0 id="basicFreqBAuto">AutoSelect</option>
	<% getWlan11bChannels(); %>
      </select>
    </td>
  </tr>
  <tr id="div_11g_channel" name="div_11g_channel">
    <td class="head"><font id="basicFreqG">Frequency (Channel)</font></td>
    <td>
      <select id="sz11gChannel" name="sz11gChannel" size="1" onChange="ChOnChange()">
	<option value=0 id="basicFreqGAuto">AutoSelect</option>
	<% getWlan11gChannels(); %>
      </select>
    </td>
  </tr>
  <tr id="div_abg_rate">
    <td class="head"><font id="basicRate">Rate</font></td>
    <td>
      <select name="abg_rate" size="1">
      </select>
    </td>
  </tr>
</table>

<table id="div_11n" name="div_11n" width="540" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD" style="display:none">
  <tr> 
    <td class="title" colspan="2" id="basicHTPhyMode">HT Physical Mode</td>
  </tr>
  <tr>
    <td class="head" id="basicHTOPMode">Operating Mode</td>
    <td>
      <input type=radio name=n_mode value="0" checked><font id="basicHTMixed">Mixed Mode&nbsp;</font>
      <input type=radio name=n_mode value="1">Green Field
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTChannelBW">Channel BandWidth</td>
    <td>
      <input type=radio name=n_bandwidth value="0" onClick="ChBwOnClick()" checked>20&nbsp;
      <input type=radio name=n_bandwidth value="1" onClick="ChBwOnClick()">20/40
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTGI">Guard Interval</td>
    <td>
      <input type=radio name=n_gi value="0" checked><font id="basicHTLongGI">long&nbsp;</font>
      <input type=radio name=n_gi value="1"><font id="basicHTAutoGI">Auto</font>
    </td>
  </tr>
  <tr>
    <td class="head">MCS</td>
    <td>
      <select name="n_mcs" size="1">
	<option value="0">0</option>
	<option value="1">1</option>
	<option value="2">2</option>
	<option value="3">3</option>
	<option value="4">4</option>
	<option value="5">5</option>
	<option value="6">6</option>
	<option value="7">7</option>
	<option value="8">8</option>
	<option value="9">9</option>
	<option value="10">10</option>
	<option value="11">11</option>
	<option value="12">12</option>
	<option value="13">13</option>
	<option value="14">14</option>
	<option value="15">15</option>
	<option value="32">32</option>
	<option value="33" selected id="basicHTAutoMCS">Auto</option>
      </select>
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTRDG">Reverse Direction Grant(RDG)</td>
    <td>
      <input type=radio name=n_rdg value="0" checked><font id="basicHTRDGDisable">Disable&nbsp;</font>
      <input type=radio name=n_rdg value="1"><font id="basicHTRDGEnable">Enable</font>
    </td>
  </tr>
  <tr name="extension_channel" id="extension_channel">
    <td class="head" id="basicHTExtChannel">Extension Channel</td>
    <td>
      <select id="n_extcha" name="n_extcha" size="1">
	<option value=1 selected>2412MHz (Channel 1)</option>
      </select>
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTSTBC">STBC</td>
    <td>
      <input type=radio name=n_stbc value="0" checked><font id="basicHTSTBCDisable">Disable&nbsp;</font>
      <input type=radio name=n_stbc value="1"><font id="basicHTSTBCEnable">Enable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTAMSDU">Aggregation MSDU(A-MSDU)</td>
    <td>
      <input type=radio name=n_amsdu value="0" checked><font id="basicHTAMSDUDisable">Disable&nbsp;</font>
      <input type=radio name=n_amsdu value="1"><font id="basicHTAMSDUEnable">Enable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTAddBA">Auto Block ACK</td>
    <td>
      <input type=radio name=n_autoba value="0" checked><font id="basicHTAddBADisable">Disable&nbsp;</font>
      <input type=radio name=n_autoba value="1"><font id="basicHTAddBAEnable">Enable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTDelBA">Decline BA Request</td>
    <td>
      <input type=radio name=n_badecline value="0" checked><font id="basicHTDelBADisable">Disable&nbsp;</font>
      <input type=radio name=n_badecline value="1"><font id="basicHTDelBAEnable">Enable</font>
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTAllowTKIP">HT Disallow TKIP</td>
    <td>
      <input type=radio name=n_disallow_tkip value="0" checked><font id="basicHTAllowTKIPDisable">Disable&nbsp;</font>
      <input type=radio name=n_disallow_tkip value="1"><font id="basicHTAllowTKIPEnable">Enable</font>
    </td>
  </tr>
  <tr id="div_2040_coexit">
    <td class="head" id="basic2040CoExit">20/40 Coexistence</td>
    <td>
      <input type=radio name=n_2040_coexit value="0" checked><font id="basic2040CoExitDisable">Disable&nbsp;</font>
      <input type=radio name=n_2040_coexit value="1"><font id="basic2040CoExitEnable">Enable</font>
    </td>
  </tr>
</table>

<table id="div_11n_plugfest" name="div_11n_plugfest" width="540" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
  <tr> 
    <td class="title" colspan="2" id="basicOther">Other</td>
  </tr>
  <tr>
    <td class="head" id="basicHTTxStream">HT TxStream</td>
    <td>
      <select name="tx_stream" size="1">
	<option value = 1>1</option>
	<option value = 2 id="div_HtTx2Stream">2</option>
      </select>
    </td>
  </tr>
  <tr>
    <td class="head" id="basicHTRxStream">HT RxStream</td>
    <td>
      <select name="rx_stream" size="1">
	<option value = 1>1</option>
	<option value = 2 id="div_HtRx2Stream">2</option>
      </select>
    </td>
  </tr>
</table>
<br />

<table width = "540" border = "0" cellpadding = "2" cellspacing = "1">
  <tr align="center">
    <td>
      <input type=submit style="{width:120px;}" value="Apply" id="basicApply"> &nbsp; &nbsp;
      <input type=reset  style="{width:120px;}" value="Cancel" id="basicCancel" onClick="window.location.reload()">
    </td>
  </tr>
</table>
</form>  

</td></tr></table>
</body>
</html>

