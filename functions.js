let but = []; 
let mes = ["-||-", "--.--.---- --:--:--"]; 
let mma = [];
let sch_title = [[],[],[],[],[]];
let restartMessage, restartTitle, resetCountMessage;
let parsed_day = 29, parsed_month = 12, parsed_year = 2020, parsed_hour = 12, parsed_week = 0;
let timeZone = 5, days_in_prev_month = 0;
let day_from, day_to;
let chart;
let rssi = 0,  s_mode = 0;
let svg_wifi = 'M0 13.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
let ttip_wifi = "";
let ver = [[],[]];
let ttip_rel = [];
let days = [['ВС', 'ПН', 'ВТ', 'СР', 'ЧТ', 'ПТ', 'СБ'], ['SU', 'MO', 'TU', 'WE', 'TH', 'FR', 'SA']];
let selectLang = [['Январь', 'Февраль', 'Март', 'Апрель', 'Май', 'Июнь', 'Июль', 'Август', 'Сентябрь', 'Октябрь', 'Ноябрь', 'Декабрь'], ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December']];
let daysOfw = [6, 0, 1, 2, 3, 4, 5];
let endOfMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
let rel_data = 33;

let dtnow;
let sec_left;
let nowValid = false;
let col_fan, col_ten;
let st_fan = 0, st_ten = 0, st_heap = 10000, ws_client = 0, rst_count = 0, ota = 0;
let st_temp = 0;
let colUnknown = 'orange', colBackgrd = '#3370B7', colError = 'red', colOn = 'green', colOff = 'red', colNorm = 'white';
let btnTry = 5;
let funcTry = 5;
let countTry = 0;
let bool = -1270;
let new_date;
let dataOption;
let onceChart = 0;
let smoothChart = 1.0;
let rightGapChart = 5;
let g;
let inputIdPrevValue = 0;

const openDefaultTab = () => {

}
	
const getData = async (data) => {
}
//let localData = new Map([['dw', [null, null]],['vr', null]]);
let localData = {};
//error error.message error.name error.stack error.at error.text	
const log = (color, i, t, v) => {
    let t2 = (t == "") ? t : " " + t + " ";
	console.log('%c' + " " + i + " " + '%c' + t2 + '%c' + " " + v, 
	'color:white; background-color:' + color, 
	'color:white; background-color: grey',
	'color:black; background-color: white');
};	
const  ShowErrorMessage = (textMessage, typeMessage) => {
/*
	document.getElementById("er_mes").style.opacity = 0.8;	
	document.getElementById("er_mes1").innerText += textMessage + "\n"; //20
	document.getElementById("er_mes2").innerText += typeMessage + "\n"; //20
	
    setInterval(function () {
		document.getElementById("er_mes").style.opacity = 0;	
		document.getElementById("er_mes1").innerText = "";
		document.getElementById("er_mes2").innerText = "";	
	}, 5000);
*/
}

const fillWithColor = (myId, myColor) => {
	switch (myId) {
	  case 'time': //time
		document.getElementById("text_time").style.color = myColor;
		break;
	  case 'temp': //temp
		document.getElementById("text_temp").style.color = myColor;
		document.getElementById("temp1ID").setAttribute('fill', myColor);	
		document.getElementById("temp2ID").setAttribute('fill', myColor);
		break;
	  case 'rssi': //rssi
		document.getElementById("text_rssi").style.color = myColor;	
		break;
	  case 'heap': //heap
		document.getElementById("text_heap").style.color = myColor;	
		break;
	  case 'fan': //fan
		document.getElementById("fanID").setAttribute('fill', myColor);
		break;
	  case 'ten': //ten
		document.getElementById("tenID1").setAttribute('fill', myColor);
		document.getElementById("tenID2").setAttribute('fill', myColor);
		document.getElementById("tenID3").setAttribute('fill', myColor);
		document.getElementById("tenID4").setAttribute('fill', myColor);
		break;		
	}	
}
const add_zero = (n) => {return n > 9 ? n: "0" + n; }
function checkForm(fobj) {document.getElementById("uploadPanel").hidden = false;document.getElementById("opacity_all").hidden = false;fobj.submit();};
const getCountSec = () => {
	if (ota)
		return;
	if (!nowValid) 
		return noDate();	
	sec_left++;	
	new_date = new Date(sec_left * 1000);
	parsed_year = new_date.getFullYear();
	parsed_month = new_date.getMonth() + 1;
	parsed_day = new_date.getDate();
	parsed_hour = new_date.getHours();
	parsed_week = new_date.getDay();
	document.getElementById("text_time").innerText = new_date.toLocaleString().replace(",", "") + " " + days[document.getElementById("selLang").value][parsed_week];
	document.getElementById("text_time").style.color = colNorm;
}
const noDate = (time) => {
	document.getElementById("text_time").innerText = mes[1];
	fillWithColor("time", colError);		
}
const updateDate = (date) => {
	dtnow = new Date(date);
	sec_left = dtnow.getTime() / 1000;
	parsed_year = dtnow.getFullYear();
	parsed_month = dtnow.getMonth() + 1;
	parsed_day = dtnow.getDate();
	parsed_hour = dtnow.getHours();
	parsed_week = dtnow.getDay();
	document.getElementById("text_time").innerHTML = dtnow.toLocaleString().replace(",", "") + " " + days[document.getElementById("selLang").value][parsed_week];
	fillWithColor("time", colNorm);
	//openChart(nowValid, parsed_year, parsed_month, parsed_day, parsed_hour, 0);		
}	
const getJson = async (jsonFile) => {
	// let jsonFile = "{\"d\":\"2023-11-29T11:59:50\",\"t\":127,\"tfv\":7,\"pwm\":[10,20,30,40,50,60],\"rel\":63,\"heap\":6300,\"rssi\":-100}"; 	
	if (ota)
		return;
	let json = JSON.parse(jsonFile);		

	// Обработка JSON.t	
	st_temp	= json.t;
	bool = st_temp == 850 || st_temp <= -550 || st_temp >= 1250 || st_temp == "undefined";		
	if (bool) {
		document.getElementById("text_temp").innerText = mes[0];
		fillWithColor("temp", colError);
		fillWithColor("fan", colError);
		fillWithColor("ten", colError);	
	} else {
		document.getElementById("text_temp").innerText = (parseFloat(st_temp) / 10).toFixed(1) + "°C";
	}
	
	// Обработка JSON.tfwv			
	nowValid  = (json.tfv > 8) ?  0 : !(json.tfv & 1) ? 0 : 1;  // valid      00000001 1
	st_fan  = (json.tfv > 8) ? colUnknown : !(json.tfv & 2) ? 0 : 1; // fan  00000010 2 
	st_ten  = (json.tfv > 8) ? colUnknown : !(json.tfv & 4) ? 0 : 1; // ten  00000100 4
	fillWithColor("fan", (bool)?colError:(st_fan)?'white':'#36343361');
	fillWithColor("ten", (bool)?colError:(st_ten)?'white':'#36343361');
	let sS = json.tfv.toString(2).padStart(3, '0');

	// Обработка JSON.d	
	if (nowValid && json.d != "2165-165-165T165:165:85"){
		updateDate(json.d);
		if (!onceChart) {
			if (document.getElementById("selMonth")) { document.getElementById("selMonth").options.selectedIndex = parsed_month - 1; }	
			await openChart(nowValid, parsed_year, parsed_month, parsed_day, parsed_hour, 0); 
			openInitialGraph();
			onceChart = 1;
		}
	} else {
		noDate();		
	}
	
	// Обработка JSON.heap			
	st_heap = json.heap;
	document.getElementById("text_heap").innerText = "HEAP: " +st_heap;
	// Обработка JSON.rssi	
	rssi = json.rssi;
	document.getElementById("text_rssi").innerText = "RSSI: " +rssi;
	
	//ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +st_heap+ '\nRST: ' +rst_count;
	if (rssi <= -110) //wifi0
		svg_wifi = 'M0 13.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
    else if (rssi <= -100) //wifi1
		svg_wifi = 'M0 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-2zm4 2a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
	else if (rssi <= -85) //wifi2
		svg_wifi = 'M0 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-2zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v5a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-5zm4 5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
	else if (rssi <= -70) //wifi3
		svg_wifi = 'M0 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-2zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v5a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-5zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v8a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-8zm4 8a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
	else //wifi4
		svg_wifi = 'M0 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-2zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v5a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-5zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v8a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-8zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v11a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-11z';			
	if (document.getElementById("wifiID")) 
		document.getElementById("wifiID").setAttribute('d', svg_wifi);
	//if (document.getElementById("ttip_wifi"))
	//	document.getElementById("ttip_wifi").setAttribute('tooltip_wifi', ttip_wifi);

	// Обработка JSON.rel	
	let iN = 1;
	for (let i = 1; i <= 6; i++) {
		document.getElementById('text_rel' + i).style.background = (json.rel > 1024)?colUnknown:!(json.rel&iN)?colOff:colOn;
		iN +=iN;
	}
	let sR = json.rel.toString(2).padStart(6, '0');			

	// Обработка JSON.pwm	
	for (let i = 1; i <= 6; i++) {
		document.getElementById('text_pwm' + i).innerHTML = i + ". " + json.pwm[i - 1] + "%";
	}
		
	log('#95B46A', "[SSE 202]", json.d.replace("T", " "), "TEMP: " + json.t + ". tfv: " + sS + ". RSSI: " + rssi + ". HEAP: " + st_heap + ". REL(654321): " + sR + ". PWM: " + json.pwm);	
	updateDate(json.d);		
}
if (!!window.EventSource) {
  var source = new EventSource('/events');
  source.addEventListener('open', function(e) {}, false);
  source.addEventListener('error', function(e) {}, false);
  source.addEventListener('message', function(e) {console.info(`[SYSTEM] ${e.data}`);}, false);
  source.addEventListener('ota', function(e) {
	try {			
		log('#D33F49', "[SSE 212]", "", "OTA: " + e.data);
        ota = JSON.parse(e.data);	
		document.getElementById("myprogressBar").innerHTML = ota + "%";
	    document.getElementById("myprogressBar").style.width = ota + "%";
	} catch(err) {
		log('red', "[SSE 217]", "", "OTA: ERROR PARSING JSON");		
	}
	}, false);
  source.addEventListener('rssi', function(e) {
	// let e = "{\"d\":\"2023-11-29T11:59:50\",\"rssi\":-100}";
	try {
        let json = JSON.parse(e.data);			
		rssi = json.rssi;
		log('#95B46A', "[SSE 224]", json.d.replace("T", " "), "RSSI: " + rssi);	
		let wi_fi;		
		document.getElementById("text_rssi").innerText = "RSSI: " +rssi;		
		if (rssi <= -110) { //wifi0
			wi_fi = 'M0 13.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
		} else if (rssi <= -100){ //wifi1
			wi_fi = 'M0 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-2zm4 2a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
		} else if (rssi <= -85){ //wifi2
			wi_fi = 'M0 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-2zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v5a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-5zm4 5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5zm4 0a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
		} else if (rssi <= -70){ //wifi3
			wi_fi = 'M0 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-2zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v5a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-5zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v8a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-8zm4 8a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 0 1h-2a.5.5 0 0 1-.5-.5z';
		} else { //wifi4
			wi_fi = 'M0 11.5a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v2a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-2zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v5a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-5zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v8a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-8zm4-3a.5.5 0 0 1 .5-.5h2a.5.5 0 0 1 .5.5v11a.5.5 0 0 1-.5.5h-2a.5.5 0 0 1-.5-.5v-11z';			
		}
		document.getElementById("wifiID").setAttribute('d', wi_fi);
		//ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +st_heap+ '\nRST: ' +rst_count;
		//document.getElementById("ttip_wifi").setAttribute('tooltip_wifi', ttip_wifi);
		updateDate(json.d);	
	} catch(err) {
		log('red', "[SSE 243]", "", "OTA: ERROR PARSING JSON");		
	}		
	}, false); 
  source.addEventListener('heap', function(e) { //+
	// let e = "{\"d\":\"2023-11-29T11:59:50\",\"heap\":6300}";
	try {
        let json = JSON.parse(e.data);			
		st_heap = json.heap;
		log('#95B46A', "[SSE 250]", json.d.replace("T", " "), "HEAP: " + st_heap);		
		document.getElementById("text_heap").innerText = "HEAP: " +st_heap;
		//ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +st_heap+ '\nRST: ' +rst_count;
		//document.getElementById("ttip_wifi").setAttribute('tooltip_wifi', ttip_wifi);	
		updateDate(json.d);
	} catch(err) {
		log('red', "[SSE 256]", "", "OTA: ERROR PARSING WS MESSAGE");		
	}	
	}, false); 
  source.addEventListener('rel', function(e) { //+ 
	// let e = "{\"d\":\"2023-11-29T11:59:50\",\"rel\":63}";
	try {  
        let json = JSON.parse(e.data);		
		log('#95B46A', "[SSE 263]", json.d.replace("T", " "), "REL(654321): " + json.rel.toString(2).padStart(6, '0'));
		let iN = 1;
		for (let i = 1; i <= 6; i++) {
			document.getElementById('text_rel' + i).style.background = (json.rel > 1024)?colUnknown:!(json.rel&iN)?colOff:colOn;
			iN +=iN;
		}
		updateDate(json.d);
	} catch(err) {
		log('red', "[SSE 271]", "", "REL: ERROR PARSING WS MESSAGE");		
	}		
    }, false);
  source.addEventListener('pwm', function(e) { //+
	// let e = "{\"d\":\"2023-11-29T11:59:50\",\"pwm\":[11,22,33,44,55,66]}"; 
	try {  
		let json = JSON.parse(e.data);		
		log('#95B46A', "[SSE 278]", json.d.replace("T", " "), "PWM(123456): " + json.pwm);
		let iN = 1;
		for (let i = 1; i <= 6; i++) {
			document.getElementById('text_pwm' + i).innerHTML = i + ". " + json.pwm[i - 1] + "%";
			iN +=iN;
		}
		updateDate(json.d);	
	} catch(err) {
		log('red', "[SSE 286]", "", "PWM: ERROR PARSING WS MESSAGE");		
	}		
	}, false);   
  source.addEventListener('tfv', function(e) { //+
	// let e = "{\"d\":\"2023-11-29T11:59:50\",\"tfv\":5}"; 	
	try {  
		if (ota)
			return;
		let json = JSON.parse(e.data);		
		//bool = st_temp == 850 || st_temp <= -550 || st_temp >= 1250 || st_temp == "undefined";		
		if (bool) {
			document.getElementById("text_temp").innerText = mes[0];
			fillWithColor("temp", colError);
			fillWithColor("fan", colError);
			fillWithColor("ten", colError);	
		} else {
			document.getElementById("text_temp").innerText = (parseFloat(st_temp) / 10).toFixed(1) + "°C";
		}
		nowValid  = (json.tfv > 8) ?  0 : !(json.tfv & 1) ? 0 : 1;  // valid      00000001 1
		st_fan  = (json.tfv > 8) ? colUnknown : !(json.tfv & 2) ? 0 : 1; // fan   00000100 2
		st_ten  = (json.tfv > 8) ? colUnknown : !(json.tfv & 4) ? 0 : 1; // ten   00001000 4
		fillWithColor("fan", (bool)?colError:(st_fan)?'white':'#36343361');
		fillWithColor("ten", (bool)?colError:(st_ten)?'white':'#36343361');
		
		let sS = json.tfv.toString(2).padStart(3, '0');
		log('#95B46A', "[SSE 312]", json.d.replace("T", " "), "tfv: " + sS);	
		updateDate(json.d);
	} catch(err) {
		log('red', "[SSE 315]", "", "PWM: ERROR PARSING WS MESSAGE");		
	}		
	}, false);   
  source.addEventListener('ALL', function(e) {	
	try {
        let json = JSON.parse(e.data);
		//log('#95B46A', "[SSE 321]", json.d.replace("T", " "), "ALL: " + e.data);		
		getJson(e.data);
	} catch(err) {
		log('red', "[SSE 324]", "", "ALL: ERROR PARSING JSON");		
	}		
	}, false);
  source.addEventListener('now', function(e) { //+
		log('#95B46A', "[SSE 331]", e.data.replace("T", " "), "NOW: " + e.data.replace("T", " "));				
		updateDate(e.data);	
	}, false); 
}
const openlang = async (cfgLang, countTry) => {
	//try {
		let url = (cfgLang == 0) ? 'RUS.json' : 'ENG.json';
		let response = await fetch(url);	
		if (!response.ok)			
			throw new Error(JSON.stringify({ procedure: "openlang", code: response.status, message: response.statusText}));
		
 		let configLang = await response.json();
		// ТАБ 0
		document.getElementById('text00').innerText = configLang.text[0][0]; //Сглаживание	
		document.getElementById('text01').innerText = configLang.text[0][1]; //Смещение	
		// ТАБ 1 загрузка месяцев
		for (let i = 0; i <= 11; i++){
			document.getElementById("m" + i).text = selectLang[cfgLang][i];
		}		
		// Загрузка Авто Вкл Выкл
		for (let j = 0; j <= 3; j++) {
			for (let i in configLang.s_mode) {
				document.getElementById("sm"+j+i).innerText = configLang.s_mode[i]; 
			}
		}
		// ТАБ 2
		for (let j = 0; j <= 5; j++)  { 
			for (let i = 1; i <= 6; i++) { 
				document.getElementById('text1'+i+j).innerText = configLang.text[1][i+5]; //Загрузка text Рассвет закат длит мин ярк макс ярк
				document.getElementById('text1'+j).innerText = configLang.text[1][j];     //Загрузка text PWM 1 2 3 4 5 6 
			}
			// ТАБ 3		
			for (let i = 1; i <= 3; i++) { 

				document.getElementById('text2'+i+j).innerText = configLang.text[2][i+5]; //Загрузка textинтервакл длит
				document.getElementById('text2'+j).innerText = configLang.text[2][j];     //Загрузка text Реле 1 2 3 4 5 6 
			}
		} 
		document.getElementById('text16').innerText = configLang.text[1][12]; //Режим всех ШИМ		
		
		document.getElementById('text26').innerText = configLang.text[2][9]; //Режим всех Реле
		document.getElementById('text27').innerText = configLang.text[2][10]; //Инверсия Реле		
		// ТАБ 4
		for (let i = 0; i <= 12; i++) { 
			document.getElementById('text3'+i).innerText = configLang.text[3][i];
		}
		for (let i in configLang.tab) {
			document.getElementById("tab"+i).innerText = configLang.tab[i]; 
		}
			
		// ТАБ 5
		for (let i = 0; i <= 7; i++) { 
			document.getElementById('text4'+i).innerText = configLang.text[4][i];
		}
 		//for (let i = 3; i <= 9; i++) {document.getElementById('input3'+i).title = configLang.popup[1];} //Требуется перезагрузка
		for (let i in configLang.but) { but[i] = configLang.but[i]; } //Сообщения кнопок
		for (let i in configLang.mes) { mes[i] = configLang.mes[i]; } //Сообщения ошибок
		
		restartMessage = configLang.popup[0]; //Перезагрузить ESP8266-12E?
		resetCountMessage = configLang.popup[2];//Сбросить счетчик перезагрузок?
		
		document.getElementById("Button11").value = but[6]; //Сохранить
		document.getElementById("Button21").value = but[6]; //Сохранить		
		document.getElementById("Button31").value = but[6]; //Сохранить
		document.getElementById("Button41").value = but[6]; //Сохранить			
		document.getElementById("Button51").value = but[4]; //Ручной ввод даты			
		document.getElementById("Button52").value = but[5]; //Автонастройка даты
		document.getElementById("Button53").value = but[6]; //Сохранить		
		document.getElementById("uploadLabel").innerHTML = but[7]; //Открыть
		document.getElementById("Button55").value = but[8]; //Загрузить
		document.getElementById("Button56").value = but[9]; //Перезагрузить	
		ShowErrorMessage("openlang", "[OK]");	
/* 	} catch(error) {	
		if (countTry >= funcTry)
			return;
		countTry += 1;
		let jsonParsed = JSON.parse(error.message);
		log('red', "["+ error.name +" "+ jsonParsed.code +"]", json.now.replace("T", " "), "\""+ jsonParsed.procedure+ "\" with error: "+ jsonParsed.message +". Try: " +countTry);	
		await new Promise((resolve, reject) => setTimeout(resolve, 2000));			
		openlang(cfgLang, countTry);		
	}	 
*/
}
const openValues = async (countTry) => {
	let url = 'configSetup.json';
	//try {	
		let response = await fetch(url);
		if (!response.ok)		
			throw new Error(JSON.stringify({ procedure: "openValues", code: response.status, message: response.statusText }));
		let configSetup = await response.json();	
		
		ver[0] = configSetup.ver[0];
		ver[1] = configSetup.ver[1];
		
		document.getElementById("text_fw").innerText=' FW: ' + ver[0];
		document.getElementById("text_fs").innerText=' FS: ' + ver[1];
		timeZone = configSetup.wifi[2];
		document.getElementById("selLang").value = configSetup.defaultLang;			
		document.getElementById("selMonth").value = parsed_month - 1;
		await openlang(configSetup.defaultLang, 0);
		
		rst_count = configSetup.restart;
		document.getElementById("text_r_count").innerText = "RESET_C: " +rst_count;		
		
		//CHART
		document.getElementById('chart0').value = configSetup.chart[0] / 10; //повторно читаем значения сглаживания и переводим во float
		document.getElementById('chart1').value = configSetup.chart[1]
		smoothChart = document.getElementById('chart0').value;
		rightGapChart = document.getElementById('chart1').value;
		
		for (let i = 0; i <= 6; i++) {		
			//PWM
			for (let j in configSetup.pwm[i]) { 
				document.getElementById('pwm'+i+j).value = configSetup.pwm[i][j]; 
			}
			//REL
			for (let j in configSetup.rel[i]) { 
				document.getElementById('rel'+i+j).value = configSetup.rel[i][j]; 
			}
		}				
		let powB = 1;
		for (let i = 0; i <= 5; i++) { //Чтение checkbox
			document.getElementById('rel7' + i).checked = parseInt((document.getElementById('rel61').value & powB) >> (i), 10); 
			//console.log(powB + " | " + parseInt((document.getElementById('rel61').value & powB) >> (i), 10));
			powB += powB;
		}

		//OPT
		for (let i in configSetup.opt) { 
			document.getElementById('opt'+i).value = configSetup.opt[i]; 
		}
		let defTab = configSetup.opt[9]; 		
		document.getElementById('opt0').value = configSetup.opt[0] / 10; //Поправка температуры / 10
		for (let i = 2; i <= 5; i++) { //повторно читаем значения гистерезиса и переводим во float
			document.getElementById('opt' + i).value = configSetup.opt[i] / 100; 
		}		
		let powC = 1;
		for (let i = 0; i <= 1; i++) { //Чтение checkbox
			document.getElementById('opt1' + i).checked = parseInt((document.getElementById('opt8').value & powC) >> (i), 10); 
			//console.log(powC + " | " + parseInt((document.getElementById('opt8').value & powC) >> (i), 10));
			powC += powC;
		}

		//WIFI		
		for (let i in configSetup.wifi) { 
			document.getElementById('wifi'+i).value = configSetup.wifi[i]; 
		}
		document.getElementById('select_smode0').options.selectedIndex = document.getElementById('pwm60').value-1;
		document.getElementById('select_smode1').options.selectedIndex = document.getElementById('rel60').value-1;
		document.getElementById('select_smode2').options.selectedIndex = document.getElementById('opt6').value-1;
		document.getElementById('select_smode3').options.selectedIndex = document.getElementById('opt7').value-1;
		document.getElementById('select_smode4').options.selectedIndex = defTab;	
		document.getElementById('tab-'+(defTab+1)).checked = 1;
/* 		let fan_on  = parseFloat(configSetup.input[2][3]/100 + (configSetup.input[2][4] / 200)).toFixed(1);
		let fan_off = parseFloat(configSetup.input[2][3]/100 - (configSetup.input[2][4] / 200)).toFixed(1);
		let ten_off  = parseFloat(configSetup.input[2][5]/100 + (configSetup.input[2][6] / 200)).toFixed(1);
		let ten_on = parseFloat(configSetup.input[2][5]/100 - (configSetup.input[2][6] / 200)).toFixed(1);

		let ttip_temp = 'COR : '+ parseFloat(configSetup.input[2][0] / 10).toFixed(1) + ' °C';
		ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +st_heap+ '\nMQTT: ' +st_mqtt+ ' WS: ' +ws_client+ '\nRST COUNT: ' +rst_count;					
		let ttip_fan = 'ON : '+ parseFloat(fan_on).toFixed(2) +' °C ' + ' ' + 'OFF: '+ parseFloat(fan_off).toFixed(2) +' °C';
		let ttip_ten = 'OFF: '+ parseFloat(ten_off).toFixed(2) +' °C ' + ' ' + 'ON : '+ parseFloat(ten_on).toFixed(2) +' °C';
		let ttip_rel1 = 'ON : ' + ttip_rel[0] + ' ' + 'OFF: ' + ttip_rel[1];
		let ttip_rel2 = 'ON : ' + ttip_rel[2] + ' ' + 'OFF: ' + ttip_rel[3]; 

		document.getElementById("ttip_wifi").setAttribute('tooltip_wifi', ttip_wifi);
		document.getElementById("ttip_temp").setAttribute('tooltip_temp', ttip_temp);
		document.getElementById("ttip_fan").setAttribute('tooltip_fan', ttip_fan);
		document.getElementById("ttip_ten").setAttribute('tooltip_ten', ttip_ten);
*/		
/* 	} catch(error) {
		if (countTry >= funcTry)
			return;
		countTry += 1;
		let jsonParsed = JSON.parse(error.message);
		log('red', "["+ error.name +" "+ jsonParsed.code +"]", json.now.replace("T", " "), "\""+ jsonParsed.procedure+ "\" with error: "+ jsonParsed.message +". Try: " +countTry);	
		await new Promise((resolve, reject) => setTimeout(resolve, 2000));			
		openValues(countTry);		
	} */
}		
//openValues().catch(error => { error.message; });	
const openInitialGraph = () => {
	let dw, vr;
	if (localStorage.getItem('localData') !== null) {
		localData = JSON.parse(localStorage.getItem("localData"));
		dw = localData.dw;
		vr = localData.vr[0];
	} else {
		dw = null;
		vr = null;
	}	
	g.updateOptions({
	    dateWindow: dw,
        valueRange: vr
	});
}
const calculateMinMax = (dw) => {
	
/* 	g.setAnnotations([{
		series: 'Temp',
		x: Date.parse('2023/05/04'),
		shortText: 'Min',
		text: 'Min value'
	}]); */
}
const openChart = async (n, y, m, d, h, countTry) => {
	let url = m + '.json';
	let jsonVal = "";

	var interactionModel = {
	  'mousedown': function(event, g, context) {
		if (event.dygraph.isZoomed()) {
		  Dygraph.defaultInteractionModel.mousedown(event, g, context);
		} else {
		  var canvasPos = Dygraph.findPos(g.canvas_);
		  context.initializeMouseDown(event, g, canvasPos);
		  context.dragStartX = event.canvasX;
		  context.dragStartY = event.canvasY;
		}
	  },
	  'mousemove': function(event, g, context) {
		if (event.dygraph.isZoomed()) {
		  Dygraph.defaultInteractionModel.mousemove(event, g, context);
		}
	  },
	  'mouseup': function(event, g, context) {
		if (event.dygraph.isZoomed()) {
		  Dygraph.defaultInteractionModel.mouseup(event, g, context);
		} else {
		  var canvasPos = Dygraph.findPos(g.canvas_);
		  context.initializeMouseDown(event, g, canvasPos);
		  context.dragEndX = event.canvasX;
		  context.dragEndY = event.canvasY;
		  context.dragging = false;
		  g.updateOptions({
			'dateWindow': g.xAxisRange()
		  });
		}
	  }
	};

	if (!n) {
		return;
	}
	try {	
		let response = await fetch(url);
		if (!response.ok)		
			throw new Error(JSON.stringify({ procedure: "openChart", code: response.status, message: response.statusText }));
		let configChart = await response.json();

		for (let i = 1; i <= (d - 1); i++) { // текущий месяц и до текущего дня				
			for (let j = 0; j <= 23; j++) {
				let val = configChart.days[i][j];
				let numb = (val == undefined || val == null || val == NaN || val == "") ? "NaN" : parseFloat(configChart.days[i][j] / 10).toFixed(1);
				jsonVal+= y + "-" + add_zero(m) + "-" + add_zero(i) + " " + add_zero(j)+":00,"+ numb +"\n"; 
			}
		} 
		for (let j = 0; j <= h; j++) { //текущий день и до текущего часа
			let val = configChart.days[d][j];
			let numb = (val == undefined || val == null || val == NaN || val == "") ? "NaN" : parseFloat(configChart.days[d][j] / 10).toFixed(1);
			jsonVal += y + "-" + add_zero(m) + "-" + add_zero(d) + " " + add_zero(j)+":00,"+ numb +"\n"; 
		}	
		jsonVal += y + "-" + add_zero(m) + "-" + add_zero(d) + " " + add_zero(h)+":10,NaN\n"; 
		jsonVal += y + "-" + add_zero(m) + "-" + add_zero(d) + " " + add_zero(h)+":20,NaN\n"; 
		jsonVal += y + "-" + add_zero(m) + "-" + add_zero(d) + " " + add_zero(h)+":30,NaN\n"; 		
		jsonVal += y + "-" + add_zero(m) + "-" + add_zero(d) + " " + add_zero(h)+":40,NaN\n"; 	
		jsonVal += y + "-" + add_zero(m) + "-" + add_zero(d) + " " + add_zero(h)+":50,NaN\n"; 			
		g = new Dygraph(
			document.getElementById("graphdiv2"),
			jsonVal,
			{
				labels: [ "Data", "Temp"],
				plotter: smoothPlotter,
				legend: 'always', // 'follow',
				color:  '#3370B7',
				strokeWidth: 2,
				drawPoints: true,	
				rightGap: rightGapChart,	
				pointSize: 2,
				showRangeSelector: true,
				labelsSeparateLines: true,
				interactionModel: Dygraph.defaultInteractionModel,
				zoomCallback: function(minDate, maxDate, yRange) {
					localData.dw[0] = minDate;
					localData.dw[1] = maxDate;
					localData.vr = yRange;
					localStorage.setItem("localData", JSON.stringify(localData));
				},
				drawCallback: function(x) {
					if (x.dateWindow_ !== null) {
						localData.dw = x.dateWindow_;
						localStorage.setItem("localData", JSON.stringify(localData));
						calculateMinMax(localData.dw);
					}
				}
				
			}
		);
		
		smoothPlotter.smoothing = smoothChart;		
		g.updateOptions({}) ;
		ShowErrorMessage("openChart", "[OK]");	
	} catch(error) {
/* 		if (countTry < funcTry) {
			countTry += 1;
			let jsonParsed = JSON.parse(error.message);
			log('red', "["+ error.name +" "+ jsonParsed.code +"]", json.now.replace("T", " "), "\""+ jsonParsed.procedure+ "\" with error: "+ jsonParsed.message +". Try: " +countTry);	
			await new Promise((resolve, reject) => setTimeout(resolve, 2000));			
			openChart(n, y, m, d, h, countTry);			
		} */
	}		
}
const ButtonClick = async (btnNumb, method, url, setText, setBody, countTry) => {
	//event.preventDefault(); 
    let message = new Object();	
	message.loading = but[0];
	message.success = but[1];
	message.failure = but[2];
	message.trying = but[3];

	btnNumb.value = message.loading;
	btnNumb.style.backgroundColor = colUnknown;	
	await new Promise((resolve, reject) => setTimeout(resolve, 200));
	try {	
		let response = await fetch(url, {
			method: method,
			headers: {
				'Content-Type': 'application/json;charset=utf-8'
			},
			body: setBody
		});
		if (response.ok) {
			let answer = await response.text();
			log('#95B46A',method.toUpperCase(), "", answer);				
			btnNumb.value = message.success;
			btnNumb.style.backgroundColor = '#008000';
			await new Promise((resolve, reject) => setTimeout(resolve, 1000));
			btnNumb.style.backgroundColor = colBackgrd;			
			btnNumb.value = setText;
		} else {
			log('red', method.toUpperCase(), "", "ERROR: " + response.status);				
			btnNumb.value = message.failure;
			btnNumb.style.backgroundColor = colError;	
			await new Promise((resolve, reject) => setTimeout(resolve, 1000));
			let errorButtonName = btnNumb.value	;
			if (countTry < btnTry) {
				countTry += 1;
				ButtonClick(btnNumb, method, url, setText, setBody, countTry);
				btnNumb.style.backgroundColor = colUnknown;	
				btnNumb.value = message.trying + ": " + countTry;
			} else {
				btnNumb.value = message.failure;
				btnNumb.style.backgroundColor = colError;	
				await new Promise((resolve, reject) => setTimeout(resolve, 1000));				
				btnNumb.style.backgroundColor = colBackgrd;	
				btnNumb.value = setText;				
			}
		}
	} catch(err) {	
		log('red', method.toUpperCase(), "", "ERROR: " + err);			
		btnNumb.style.backgroundColor = colError;	
		btnNumb.value = message.failure;
		await new Promise((resolve, reject) => setTimeout(resolve, 1000));
		if (countTry < btnTry) {	
			countTry += 1;	
			ButtonClick(btnNumb, method, url, setText, setBody, countTry);	
			btnNumb.style.backgroundColor = colUnknown;	
			btnNumb.value = message.trying + ": " + countTry;
		} else {
			btnNumb.value = message.failure;
			btnNumb.style.backgroundColor = colError;	
			await new Promise((resolve, reject) => setTimeout(resolve, 1000));			
			btnNumb.style.backgroundColor = colBackgrd;	
			btnNumb.value = setText;		
		}
	
		
	}
}
const init = () => {
	let btn00 = document.getElementById("selLang");
	let btn01 = document.getElementById("selMonth");
	let btn11 = document.getElementById("Button11");
	let btn21 = document.getElementById("Button21");
	let btn22 = document.getElementById("select_smode0");
	let btn31 = document.getElementById("Button31");
	let btn32 = document.getElementById("select_smode1");
	let btn41 = document.getElementById("Button41");
	let btn42 = document.getElementById("select_smode2");
	let btn43 = document.getElementById("select_smode3");	
	let btn44 = document.getElementById("select_smode4");
	let btn51 = document.getElementById("Button51");
	let btn52 = document.getElementById("Button52");
	let btn53 = document.getElementById("Button53");
	//let btn55 = document.getElementById("Button55");
	let btn56 = document.getElementById("Button56");

	btn00.addEventListener("change", function(event){ 
		event.preventDefault();
		openlang(this.value, 0);
	});

	btn01.addEventListener("change", function(event){ 
		event.preventDefault();
		let selM = document.getElementById("selMonth").options.selectedIndex + 1;
		let m, d, h;
		if (selM != parsed_month) {
			m = selM;
			d = endOfMonth[selM - 1];
			h = 23;
		} else {
		    m = parsed_month;
			d = parsed_day;
			h = parsed_hour;			
		}
		openChart(nowValid, parsed_year, m, d, h, 0);	
	});
	btn11.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
		smoothChart = document.getElementById('chart0').value;
		smoothPlotter.smoothing = smoothChart;		
		rightGapChart = document.getElementById('chart1').value;
		g.updateOptions({
			rightGap: rightGapChart
		});
			jsonData = 'chart[0]=' + document.getElementById('chart0').value + '&chart[1]=' + document.getElementById('chart1').value;
		ButtonClick(btn11, "get", 'smooth?'+jsonData, but[6], undefined, 1); 		
	});
 	btn21.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
		for (let i = 0; i <= 5; i++) {		
			for (let j = 0; j <= 5; j++) { 
				jsonData += 'pwm['+i+']['+j+']='+document.getElementById('pwm'+i+j).value+'&';
			}
		}
		jsonData += 'pwm[6][0]='+document.getElementById('pwm60').value;
 
		ButtonClick(btn21, "get", 'save_pwm?'+jsonData, but[6], undefined, 1); 
	});	
	btn22.addEventListener("change", function(event){ 
		event.preventDefault();
		document.getElementById('pwm60').value = document.getElementById('select_smode0').options.selectedIndex+1;
	});
/* 		s_mode = 0; //max 262143
		for (let i = 0; i <= 8; i++) {
			s_mode += (document.getElementById("select_smode" + i).options.selectedIndex + 1) << (i * 2);	
			//console.log(`${i} | value: ${document.getElementById("select_smode" + i).options.selectedIndex} s_mode: ${s_mode} ${s_mode.toString(2)}`);
		} */
	btn31.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let jsonData = '';
		let chk = 0;
		
		for (let i = 0; i <= 5; i++) {
			chk += (document.getElementById("rel7" + i).checked) << (i);
		}
		document.getElementById('rel61').value = chk;
		
		for (let i = 0; i <= 5; i++) {		
			for (let j = 0; j <= 2; j++) { 
				jsonData += 'rel['+i+']['+j+']='+document.getElementById('rel'+i+j).value+'&';
			}
		}
		jsonData += 'rel[6][0]='+document.getElementById('rel60').value+'&';
		jsonData += 'rel[6][1]='+document.getElementById('rel61').value;
/* 		let fan_on  = parseFloat(document.getElementById("input213").value) + (parseFloat(document.getElementById("input214").value) / 2);
		let fan_off = parseFloat(document.getElementById("input213").value) - (parseFloat(document.getElementById("input214").value) / 2);
		let ten_off = parseFloat(document.getElementById("input215").value) + (parseFloat(document.getElementById("input216").value) / 2);
		let ten_on  = parseFloat(document.getElementById("input215").value) - (parseFloat(document.getElementById("input216").value) / 2);
		let ttip_fan = 'ON : ' + parseFloat(fan_on).toFixed(2) + ' °C OFF: ' + parseFloat(fan_off).toFixed(2) + ' °C';
		let ttip_ten = 'OFF: ' + parseFloat(ten_off).toFixed(2) + ' °C ON : ' + parseFloat(ten_on).toFixed(2) + ' °C';
		let ttip_temp = 'COR : ' + parseFloat(document.getElementById('input20').value).toFixed(1) + ' °C';	
		let ttip_led = 'MAX: '+document.getElementById("input21").value+' % ' + ' ' + ' MIN: '+document.getElementById("input22").value+' %';		
		
		document.getElementById("ttip_temp").setAttribute('tooltip_temp', ttip_temp);
		document.getElementById("ttip_fan").setAttribute('tooltip_fan', ttip_fan);
		document.getElementById("ttip_ten").setAttribute('tooltip_ten', ttip_ten);			
		document.getElementById("ttip_led").setAttribute('tooltip_led', ttip_led);	
		log('blue',"[OPTION 805]", "INV", chk.toString(2));	 */		
		//console.log(`INV: ${chk.toString(2)}`);
		ButtonClick(btn31, "get", 'save_rel?'+jsonData, but[6], undefined, 1); 
	});	
	btn32.addEventListener("change", function(event){ 
		event.preventDefault();
		document.getElementById('rel60').value = document.getElementById('select_smode1').options.selectedIndex+1;
	});
	btn41.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let jsonData = '';
		let chk = 0;
		
		for (let i = 0; i <= 1; i++) {
			chk += (document.getElementById("opt" + (10+i)).checked) << (i);
		}
		document.getElementById('opt8').value = chk;	
		
		for (let i = 0; i <= 9; i++) {
			jsonData += 'opt['+i+']='+document.getElementById('opt'+i).value+'&';
		}
		ButtonClick(btn41, "get", 'save_opt?'+jsonData.substring(0, jsonData.length-1), but[6], undefined, 1); 
	});	
	btn42.addEventListener("change", function(event){ 
		event.preventDefault();
		document.getElementById('opt6').value = document.getElementById('select_smode2').options.selectedIndex+1;
	});	
	btn43.addEventListener("change", function(event){ 
		event.preventDefault();
		document.getElementById('opt7').value = document.getElementById('select_smode3').options.selectedIndex+1;
	});	
	btn44.addEventListener("change", function(event){ 
		event.preventDefault();
		document.getElementById('opt9').value = document.getElementById('select_smode4').options.selectedIndex;
	});		
	btn51.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
		for (let i = 0; i <= 2; i++) {
			jsonData += 'wifi['+i+']='+document.getElementById('wifi'+i).value+'&';
		}
		ButtonClick(btn51, "get", 'man_date?'+jsonData.substring(0, jsonData.length-1), but[4], undefined, 1); 
	});
	btn52.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let jsonData = 'wifi[2]=' + document.getElementById("wifi2").value;
		ButtonClick(btn52, "get", 'auto_sync?'+jsonData, but[5], undefined, 1); 
	});		
	btn53.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
  		for (let i = 3; i <= 9; i++) {
			jsonData += 'wifi['+i+']='+document.getElementById('wifi'+i).value+'&';
		} 
  		ButtonClick(btn53, "get", '/save_wifi?' + jsonData.substring(0, jsonData.length-1), but[6], undefined, 1); 
  		//ButtonClick(btn53, "post", '/save_wifi', jsonData.substring(0, jsonData.length-1), 1);
	});	
	btn56.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let device = confirm(restartMessage)?1:0;
        if (device == 1) {
			ButtonClick(btn56, "get", '/restart?device='+device, but[9], undefined, 1); 		
			//ButtonClick(btn56, "post", '/restart', 'device='+device, 1); 		
			setTimeout(() => {window.location.reload(true);}, 10000);
		}
	});
	document.getElementById("rst_c").addEventListener("click", function(event){ 
		event.preventDefault(); 
		rst_count=confirm(resetCountMessage)?1:0;
		let btn00 = document.getElementById("rst_c");
		if (rst_count) {
			document.getElementById("text_r_count").innerText = "RESET_C: " +0;	
		}
		//ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +st_heap+ '\nWS: ' +ws_client+ '\nRST COUNT: 0';			
		//document.getElementById("ttip_wifi").setAttribute('tooltip_wifi', ttip_wifi);
		ButtonClick(btn00, "get", '/rst_count?restart=' + rst_count, but[6], undefined, 1);
	});
}
window.onerror = function(message, url, lineNumber) {
	console.log("Поймана ошибка, выпавшая в глобальную область!\n" +
	"Сообщение: " + message + "\n(" + url + ":" + lineNumber + ")");
};
window.onload = function() {
	openDefaultTab();
	openValues(0);		
	init();	
	let jsonFile = "{\"d\":\"2023-11-29T11:59:50\",\"t\":127,\"tfv\":7,\"pwm\":[10,20,30,40,50,60],\"rel\":63,\"heap\":6300,\"rssi\":-100}"; 	
	getJson(jsonFile);
	getCountSec();
    setInterval(function () { getCountSec();}, 1000);	
}
/*
document.addEventListener("visibilitychange", function(){
	if (document.hidden){
		console.log('Вкладка не активна');
	} else {
		console.log('Вкладка активна');    
	}
});
*/
/*
let eventType = 1;	
document.addEventListener('touchstart', handleTouchStart, false);
document.addEventListener('touchmove', handleTouchMove, false);
document.addEventListener('touchend', handleTouchEnd, false);
let xDown = null;
let yDown = null;
let xDiff = null;
let yDiff = null;
let timeDown = null;
let startEl = null;
*/
/*
function handleTouchEnd(e) {
	if (document.getElementById("opacity_all").hidden) { 
		if (startEl !== e.target) return;
		let swipeThreshold = 200;
		let swipeTimeout = 1000;
		let timeDiff = Date.now() - timeDown;
		if (Math.abs(xDiff) > Math.abs(yDiff)) { // most significant
			if (Math.abs(xDiff) > swipeThreshold && timeDiff < swipeTimeout) {
				if (xDiff > 0) {
					console.log("xDiff > 0");
					//eventType ++;				
					//if (eventType > 5) eventType = 1;
				}
				else {
					console.log("xDiff < 0");
					//eventType --;	
					//if (eventType < 1) eventType = 5;
				}
			}
			//changeZindex(eventType);
			//document.getElementById('tab_'+ eventType).checked = true;		
			//if (eventType==1) {updateChart();}
		}		
		//if (eventType !== '') {
		//	startEl.dispatchEvent(new CustomEvent(eventType, { bubbles: true, cancelable: true }));
		//}
		xDown = null;
		yDown = null;
		timeDown = null;
	}
};

function handleTouchStart(e){if (e.target.getAttribute('data-swipe-ignore') === 'true') return;startEl = e.target;timeDown = Date.now();xDown = e.touches[0].clientX;yDown = e.touches[0].clientY;xDiff = 0;yDiff = 0;}
function handleTouchMove(e) {if (!xDown || !yDown) return;let xUp = e.touches[0].clientX;let yUp = e.touches[0].clientY;xDiff = xDown - xUp;yDiff = yDown - yUp;}

*/