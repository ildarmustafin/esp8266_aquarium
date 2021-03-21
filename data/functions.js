let but = []; 
let mes = ["-||-", "RTC ERROR"]; 
let min_max_aver = [];
let sch_title = [[],[]];
let defaultLang, restartMessage, restartTitle;
let parsed_day = 29;
let parsed_month = 12;
let parsed_year = 2020;
let parsed_hour = 12;
let parsed_week = 0;
let timeZone = 5;
let day_from, day_to;
let days_in_prev_month = 0;
let chart, rssi;
let ttip_wifi = "";
let s_mode = [[],[]];
let ver = [[],[]];
let ttip_rel = [];
let min, max, aver, aver_sum;
let load_values = 0;
let data;
let device;
let perc = 0; 
var days = [['ВС', 'ПН', 'ВТ', 'СР', 'ЧТ', 'ПТ', 'СБ'], ['SU', 'MO', 'TU', 'WE', 'TH', 'FR', 'SA']];
//var daysENG = ['SU', 'MO', 'TU', 'WE', 'TH', 'FR', 'SA'];
var daysOfw = [6, 0, 1, 2, 3, 4, 5];
//let test_json = "{\"now\":\"2021-03-07T16:18:59\",\"t\":-142.7,\"led\":0,\"rssi\":-89,\"bf\":255}";
let dtnow;
let sec_left;
let nowisValid;
let ws_client;
let fan;
let ten;
let rel1;
let rel2;
let mqtt;
let led;
let heap;
let ota = 0;

window.onload = function() {
	openValues();
	init();	
	getCountSec();
}

function getCountSec(){
	if (ota == 0) {
    	if (nowisValid != 0) {    
	   		sec_left++;
      		var new_date = new Date(sec_left * 1000);
	   		document.getElementById("text_t3").innerHTML = new_date.toLocaleString()+' '+days[parsed_week];
	   		document.getElementById("text_t3").style="color:white;font-size:20px;line-height:20px;text-align:center;font-weight:bold;";
    		window.setTimeout(function () { getCountSec();}, 1000);
    	} else {
			noDate();	
		}
 	}
}
function noDate() {
	document.getElementById("text_t3").innerHTML = mes[1];
	console.log('[ERROR]', mes[1]);
	document.getElementById("text_t3").style="color:red;font-size:20px;width:65%;text-align:center;vertical-align:middle;display:table-cell;";
	document.getElementById("select_smode0").options.selectedIndex = 2;
	document.getElementById("select_smode1").options.selectedIndex = 2;
	document.getElementById("select_smode2").options.selectedIndex = 2;
	document.getElementById("select_smode0").disabled = true;
	document.getElementById("select_smode1").disabled = true;
	document.getElementById("select_smode2").disabled = true;
}
function getJson(data) {
	if (ota == 0) {
		let json = JSON.parse(data);	
		ws_client  = !(json.bf&64)?'0':'1'; // mqtt  01000000 64
		fan 	   = !(json.bf&32)?'0':'1'; // fan   00100000 32
		ten 	   = !(json.bf&16)?'0':'1'; // ten   00010000 16
		rel1 	   = !(json.bf&8)?'0':'1';  // rel1  00001000 8
		rel2 	   = !(json.bf&4)?'0':'1';  // rel2  00000100 4
		mqtt 	   = !(json.bf&2)?'0':'1';  // mqtt  00000010 2
		nowisValid = !(json.bf&1)?'0':'1';  // valid 00000001 1
		rssi = json.rssi;
		heap = json.heap;
		if (nowisValid == 0){
			noDate();
		} else {
			for (let i = 0; i <= 2; i++) {
				if (document.getElementById('select_smode'+(parseInt(i))).disabled) {
					document.getElementById('select_smode'+(parseInt(i))).disabled = false;
					document.getElementById('select_smode'+(parseInt(i))).options.selectedIndex = s_mode[i];				
				}	
			}
			dtnow = new Date(json.now);
	    	sec_left = dtnow.getTime() / 1000;  		
			parsed_year = dtnow.getFullYear();
			parsed_month = dtnow.getMonth() + 1;
			parsed_day = dtnow.getDate();
			parsed_hour = dtnow.getHours();	days
			parsed_week = dtnow.getDay();	
			openChart(); 
			document.getElementById("text_t3").innerHTML = dtnow.toLocaleString()+' '+days[parsed_week];
			document.getElementById("text_t3").style="color:white;font-size:20px;line-height:20px;text-align:center;font-weight:bold;";
		}
		document.getElementById("text_t5").innerHTML = json.led + "%";	
		document.getElementById("Picture3").setAttribute("class", (json.led === 0) ? 'led_0 mx-auto d-block' : 'led_1 mx-auto d-block');
		document.getElementById("Picture6").setAttribute("class", 'relay1_' + rel1 +' mx-auto d-block');
		document.getElementById("Picture7").setAttribute("class", 'relay2_' + rel2 +' mx-auto d-block');  
		document.getElementById("Picture4").setAttribute("class", 'fan_' + fan +' mx-auto d-block'); 
		document.getElementById("Picture5").setAttribute("class", 'ten_' + ten +' mx-auto d-block');		
		class_wifi = rssi<=-110?'wifi1 mx-auto d-block':rssi<=-100?'wifi2 mx-auto d-block':rssi<=-85?'wifi3 mx-auto d-block':rssi<=-70?'wifi4 mx-auto d-block':rssi<=-50?'wifi5 mx-auto d-block':'wifi6 mx-auto d-block';
		document.getElementById("Picture2").setAttribute("class", class_wifi);
		ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +heap+ '\nMQTT: ' +mqtt+ ' WS: ' +ws_client;
		document.getElementById("Picture2").setAttribute('tooltip_wifi', ttip_wifi);
		if (json.t == 85.0 || json.t <= -55.0 || json.t >= 125.0 || json.t == "undefined") {
			document.getElementById("text_t2").innerHTML = mes[0];		
			document.getElementById("Picture1").setAttribute("class", "tempna  mx-auto d-block");	
		} else {
			document.getElementById("text_t2").innerHTML = json.t + "°C";
			document.getElementById("Picture1").setAttribute("class", "temp  mx-auto d-block");
		} 
	}
}
if (!!window.EventSource) {
  var source = new EventSource('/events');
  source.addEventListener('open', function(e) {}, false);
  source.addEventListener('error', function(e) {}, false);
  source.addEventListener('message', function(e) {console.log("[SYSTEM]", e.data);}, false);
  source.addEventListener('ota', function(e) {console.log("[SSE] OTA:", e.data);ota = e.data;document.getElementById("progressId").innerHTML = ota + "%";document.getElementById("progressId").style.width = ota + "%";}, false);
  source.addEventListener('ws',  function(e) {console.log("[SSE] WS:", e.data);ws_client = e.data;ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +heap+ '\nMQTT: ' +mqtt+ ' WS: ' +ws_client;document.getElementById("Picture2").setAttribute('tooltip_wifi', ttip_wifi);}, false);
  source.addEventListener('mqtt', function(e) {console.log("[SSE] MQTT:", e.data);mqtt = e.data;ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +heap+ '\nMQTT: ' +mqtt+ ' WS: ' +ws_client;document.getElementById("Picture2").setAttribute('tooltip_wifi', ttip_wifi);}, false);  
  source.addEventListener('rssi', function(e) {console.log('[SSE] RSSI:', e.data);rssi = e.data;class_wifi = rssi<=-110?'wifi1 mx-auto d-block':rssi<=-100?'wifi2 mx-auto d-block':rssi<=-85?'wifi3 mx-auto d-block':rssi<=-70?'wifi4 mx-auto d-block':rssi<=-50?'wifi5 mx-auto d-block':'wifi6 mx-auto d-block';document.getElementById("Picture2").setAttribute("class", class_wifi);ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +heap+ '\nMQTT: ' +mqtt+ ' WS: ' +ws_client;document.getElementById("Picture2").setAttribute('tooltip_wifi', ttip_wifi);}, false); 
  source.addEventListener('heap', function(e) {console.log('[SSE] HEAP:', e.data);heap = e.data;ttip_wifi = 'RSSI: '+rssi+' dBm\n'+ 'HeapMin: ' +heap+ '\nMQTT: ' +mqtt+ ' WS: ' +ws_client;document.getElementById("Picture2").setAttribute('tooltip_wifi', ttip_wifi);}, false); 
  source.addEventListener('rel1', function(e) {console.log('[SSE] REL1:', e.data);rel1 = e.data;document.getElementById("Picture6").setAttribute("class", 'relay1_' + rel1 +' mx-auto d-block');}, false); 
  source.addEventListener('rel2', function(e) {console.log('[SSE] REL2:', e.data);rel2 = e.data;document.getElementById("Picture7").setAttribute("class", 'relay2_' + rel2 +' mx-auto d-block');}, false); 
  source.addEventListener('fan', function(e) {console.log('[SSE] FAN:', e.data);fan = e.data;document.getElementById("Picture4").setAttribute("class", 'fan_' + fan +' mx-auto d-block');}, false); 
  source.addEventListener('ten', function(e) {console.log('[SSE] TEN:', e.data);ten = e.data;document.getElementById("Picture5").setAttribute("class", 'ten_' + ten +' mx-auto d-block');}, false); 
  source.addEventListener('led', function(e) {console.log('[SSE] LED:', e.data);led = e.data;document.getElementById("text_t5").innerHTML = led + "%";document.getElementById("Picture3").setAttribute("class", (led === 0) ? 'led_0 mx-auto d-block' : 'led_1 mx-auto d-block');}, false);   
  source.addEventListener('temp', function(e) {let t = e.data.slice(0,-2);console.log('[SSE] TEMP:', t);if (t == 85.0 || t <= -55 || t >= 125 || t == "undefined") {document.getElementById("text_t2").innerHTML = mes[0];document.getElementById("Picture1").setAttribute("class", "tempna  mx-auto d-block");} else {document.getElementById("text_t2").innerHTML = t + "°C";document.getElementById("Picture1").setAttribute("class", "temp  mx-auto d-block");}}, false);
  source.addEventListener('ALL', function(e) {console.log('[SSE] ALL:', e.data);getJson(e.data);}, false);
  source.addEventListener('now', function(e) {console.log('[SSE] NOW:', e.data);dtnow = new Date(e.data);sec_left = dtnow.getTime() / 1000;parsed_year = dtnow.getFullYear();parsed_month = dtnow.getMonth() + 1;parsed_day = dtnow.getDate();parsed_hour = dtnow.getHours();parsed_week = dtnow.getDay();openChart();document.getElementById("text_t3").innerHTML = dtnow.toLocaleString()+' '+days[parsed_week];document.getElementById("text_t3").style="color:white;font-size:20px;line-height:20px;text-align:center;font-weight:bold;";}, false); 
}
function chartInputChange(n){let num = document.getElementById("input00").value;(n>0&&num<28)?num++:(n<0&&num>2)?num--:num;document.getElementById("input00").value = num;openChart();}
function changeLang(selectedLang) {event.preventDefault();openlang(selectedLang);let jsonData = 'defaultLang=' + selectedLang;let xhttp = new XMLHttpRequest();xhttp.open("GET", "lang?"+jsonData, true);xhttp.send();}
function changeZindex(n){eventType	= n; let elem = document.getElementsByClassName("tabs");elem[0].style.zIndex = n * 10;}
function add_zero(n){return n > 9 ? n: "0" + n; }
function checkForm(fobj) {document.getElementById("uploadPanel").hidden = false;document.getElementById("opacity_all").hidden = false;fobj.submit();};
function openChart(){
	n = document.getElementById("input00").value;
    if (!load_values && !n) {
		setTimeout(function () { openChart(); }, 1000);
	} else {

		parsed_day>=n?(day_from=parsed_day-n+1,day_to=parsed_day):(days_in_prev_month=new Date(parsed_year,parsed_month-1,0).getDate(),day_from=days_in_prev_month-n+parsed_day+1,day_to=parsed_day);

		let xhttp = new XMLHttpRequest();
		xhttp.open("GET", "configChart.json", true);
		xhttp.timeout = 10000;	
		xhttp.ontimeout = function (e) {
			console.log("[TIMEOUT] UpdateChart");
			window.setTimeout(function () {openChart();}, 2000);					
		};
		xhttp.onerror = function() {
			console.log("[ERROR] Reopen UpdateChart");	
			window.setTimeout(function () {openChart();}, 2000);			
		};
		xhttp.onabort = function () {
		  console.log("[ABORT] UpdateChart");
		};	
		xhttp.onload = function () {
			if (this.readyState == 4 && this.status == 200) {
				let jsonfile = JSON.parse(this.responseText);
				let jsonVal = {"date":[],"values":[]};
				let k = day_from;
				let time_ost = (parsed_hour / 2);	

				if (day_from > day_to) {
					k = 1;
					for (let i = day_from; i <= days_in_prev_month; i++) {
						for (let j = 0; j <= 11; j++) {
							jsonVal.date.push(add_zero(i)+"."+add_zero(parsed_month-1)+"."+parsed_year+"_" + add_zero(j*2)+":00");
							jsonVal.values.push(jsonfile.days[i][j]);
						}
					} 	
				}
				for (let i = k; i <= (day_to-1); i++) {
					for (let j = 0; j <= 11; j++) {
							jsonVal.date.push(add_zero(i)+"."+add_zero(parsed_month)+"."+parsed_year+"_" + add_zero(j*2)+":00");
							jsonVal.values.push(jsonfile.days[i][j]);
					}
				} 
				for (let j = 0; j <= time_ost; j++) {
						jsonVal.date.push(add_zero(parsed_day)+"."+add_zero(parsed_month)+"."+parsed_year+"_" + add_zero(j*2)+":00");
						jsonVal.values.push(jsonfile.days[day_to][j]);
				}	
				min = jsonVal.values[0];
				max = min;			
				aver_sum = 0;
				for (i in jsonVal.values) {
					if (jsonVal.values[i] > max && jsonVal.values[i] != 85) max = jsonVal.values[i];
					if (jsonVal.values[i] < min && jsonVal.values[i] != -127) min = jsonVal.values[i];
					aver_sum += parseInt(jsonVal.values[i]);
				} 
				aver = (aver_sum / jsonVal.values.length).toFixed(2);
				if (min != undefined && max != undefined) {
					document.getElementById('text01').value = min_max_aver[1]+' '+min+' °C';
					document.getElementById('text02').value = min_max_aver[2]+' '+max+' °C';
					document.getElementById('text03').value = min_max_aver[3]+' '+aver+' °C';
				}			
				data = {
					labels: jsonVal.date, 
					series: [jsonVal.values]
				};
				updateChart();
			}
		};
		xhttp.send();
	}
}
function updateChart() {
	if (nowisValid == 1) {
		document.getElementById("minmax").hidden = false;
		document.getElementById("chart1").hidden = false;
		
		let options = {
			height: '400px',
			axisX: {
				showLabel: false
			},	
			chartPadding: {
				top: 15,
				right: 5,
				bottom: -25,
				left: 10
			},				
			plugins: [
				Chartist.plugins.lineTooltip({
					appendTooltipContent: function (tooltip, data, pluginOptions) {
						var title = document.createElement('div');
						title.setAttribute('class', 'ct-tooltip-title');
						title.innerHTML = data.label;
						tooltip.appendChild(title);
						var seriesLabel;
						data.series.forEach(function (value) {
							seriesLabel = pluginOptions.createSeriesLabel({
								label  : value
							}, pluginOptions);
							tooltip.appendChild(seriesLabel);
						});
					},
					createSeriesLabel: function (options, pluginOptions) {
						var seriesLabel = document.createElement('div');
						seriesLabel.setAttribute('class', 'ct-tooltip-series-label');
						seriesLabel.innerHTML = '\
						<span class="ct-tooltip-legend ct-tooltip-legend-a"></span>\
						<span class="ct-tooltip-label">\
						' + options.label + '\
						' + " °C" + '\
						</span>';
						return seriesLabel;
					}		
				})
			]
		};
		chart = new Chartist.Line('.ct-chart', data, options);
		chart.on('draw', function (data) {
			if (data.type === 'point') {
				let n = document.getElementById('input00').value;
				if (n >= 7) {
					data.element._node.setAttribute('style','stroke-width: 0px;');
				} else {
					data.element._node.setAttribute('style','stroke-width: 6px;');
				}	  
			}
		});
		var positionTooltip = function (tooltip, point) {
			tooltip.style.top  = (pointPosition.top - tooltipSize.height - 16) + 'px';
			if (point.index <= points.length / 2) {
				tooltip.style.left = (pointPosition.left - tooltipSize.width / 2 + 65) + 'px';	
				tooltip.classList.toggle('left');
			} else {
				tooltip.style.left = (pointPosition.left - tooltipSize.width / 2 - 70) + 'px';
				tooltip.classList.toggle('right');
			}
		};	
	} else {
		//console.log('HIDE');
		//document.getElementById("text01").hidden = true;
		//document.getElementById("text02").hidden = true;
		//document.getElementById("text03").hidden = true;		
		//document.getElementById("chart1").hidden = true;		
	}	
}
function openSchedule() {
	event.preventDefault();
	let selIndex = document.getElementById("select_schedule").options.selectedIndex;	
	let xhttp = new XMLHttpRequest();
	xhttp.open("GET", 'configSetup.json', true);
    xhttp.timeout = 10000;
	xhttp.ontimeout = function (e) {
		console.log("[TIMEOUT] OpenSchedule");
		window.setTimeout(function () {openSchedule();}, 2000);		
	};
	xhttp.onerror = function() {
	    console.log("[ERROR] Reopen OpenSchedule");	
		window.setTimeout(function () {openSchedule();}, 2000);				
	};
	xhttp.onabort = function () {
	  console.log("[ABORT] OpenSchedule");
	};	
	xhttp.onload = function () {
		if (this.readyState == 4 && this.status == 200) {
			let configSetup = JSON.parse(this.responseText);	
			for (let i = 0; i <= 5; ++i) {
				document.getElementById('sch_title'+(parseInt(i))).value = sch_title[selIndex][i];
			}
			for (let j = 0; j <= 3; j++) {
				for (let i = 0; i <= 6; i++) {
					document.getElementById('schedule'+(parseInt(i))+(parseInt(j))).value = configSetup.schedule[selIndex][i][j];
				}
				ttip_rel[j] = configSetup.schedule[1][daysOfw[parsed_week]][j];
			}
			let ttip_rel1 = 'ON : ' + ttip_rel[0] + ' OFF: ' + ttip_rel[1];
			document.getElementById("Picture6").setAttribute('tooltip_rel1', ttip_rel1);
			let ttip_rel2 = 'ON : ' + ttip_rel[2] + ' OFF: ' + ttip_rel[3];
			document.getElementById("Picture7").setAttribute('tooltip_rel2', ttip_rel2);	
		}		
	};
	xhttp.send();
}
function openlang(lng) {
    let lang_path = lng+'.json';
	let selIndex = document.getElementById("select_schedule").options.selectedIndex;	
	let xhttp = new XMLHttpRequest();
	xhttp.open("GET", lang_path, true);
    xhttp.timeout = 10000;	
	xhttp.ontimeout = function (e) {
		console.log("[TIMEOUT] Openlang");	
		window.setTimeout(function () {openlang(lng);}, 2000);		
	};
	xhttp.onerror = function() {
	    console.log("[ERROR] Reopen Openlang");	
		window.setTimeout(function () {openlang(lng);}, 2000);
	};
	xhttp.onabort = function () {
	  console.log("[ABORT] Openlang");
	};	
	xhttp.onload = function () {
		if (this.readyState == 4 && this.status == 200) {
			let configLang = JSON.parse(this.responseText);
			document.getElementById("text_s0").innerHTML = "&nbsp;&nbsp;&nbsp;" + configLang.sch_title[0][0]; //Расписание светодиодов
			document.getElementById("text_s1").innerHTML = "&nbsp;&nbsp;&nbsp;" + configLang.sch_title[0][1]; //Расписание реле

			for (let j = 0; j <= 2; j++) { // Загрузка текста "Авто", "Вкл", "Выкл"
				for (let i = 0; i <= 2; i++) {
					document.getElementById("sm"+(parseInt(i))+(parseInt(j))).innerHTML = configLang.s_mode[j]; 
				}
			}
			document.getElementById('text00').innerHTML = configLang.text[0][0]; //Загрузка текста "Количество дней"
			for (let i = 1; i <= 3; i++)  {                                       //Загрузка текста "Мин", "Макс", "Среднее"
				min_max_aver[i] = configLang.text[0][i]; 
			}
			document.getElementById('text01').value = min_max_aver[1]+' -- °C';
			document.getElementById('text02').value = min_max_aver[2]+' -- °C';
			document.getElementById('text03').value = min_max_aver[3]+' -- °C';
			for (let i = 1; i <= 4; i++) { //Загрузка всех text
				for (let j in configLang.text[i])  { 
					document.getElementById('text'+(parseInt(i))+(parseInt(j))).innerHTML = configLang.text[i][j]; 
				}
			}
			for (let i = 3; i <= 9; i++) { document.getElementById('input3'+(parseInt(i))).title = configLang.popup[1];   } //Требуется перезагрузка
			for (let i in configLang.but) { but[i] = configLang.but[i]; } //Сообщения кнопок
			for (let i in configLang.mes) { mes[i] = configLang.mes[i]; } //Сообщения ошибок
			for (let i = 0; i <= 5; i++) { 								  //Заголовки расписания
				sch_title[0][i] = configLang.sch_title[1][i];
				sch_title[1][i] = configLang.sch_title[2][i];
				document.getElementById('sch_title'+(parseInt(i))).value = sch_title[selIndex][i];
			}
			restartMessage = configLang.popup[0]; //Перезагрузить ESP8266-12E?
			document.getElementById("Button1").value = but[5]; //Сохранить
			document.getElementById("Button2").value = but[5]; //Сохранить
			document.getElementById("Button3").value = but[5]; //Сохранить
			document.getElementById("Button4").value = but[5]; //Сохранить
			document.getElementById("Button5").value = but[5]; //Сохранить			
			document.getElementById("Button6").value = but[3]; //Ручной ввод даты			
			document.getElementById("Button7").value = but[4]; //Автонастройка даты
			document.getElementById("Button8").value = but[5]; //Сохранить		
			document.getElementById("Button9").innerHTML = but[6]; //Открыть
			document.getElementById("Button10").value = but[7]; //Загрузить
			document.getElementById("Button11").value = but[8]; //Перезагрузить
		}
	};
	xhttp.send();	
}
function openValues() {
    let selectList = document.getElementById("select_lang");
	let selIndex = document.getElementById("select_schedule").options.selectedIndex;	
	let xhttp = new XMLHttpRequest();
    xhttp.open("GET", 'configSetup.json', true);
	xhttp.timeout = 10000;
	xhttp.ontimeout = function (e) {
		console.log("[TIMEOUT] OpenValues");
		window.setTimeout(function () {openValues();}, 2000);
	};
	xhttp.onerror = function() {
	    console.log("[ERROR] OpenValues");	
		window.setTimeout(function () {openValues();}, 2000);
	};
	xhttp.onabort = function () {
	  console.log("[ABORT] OpenValues");
	};	
	xhttp.onload = function () {
		if (this.readyState == 4 && this.status == 200) {
			let configSetup = JSON.parse(this.responseText);
			for (let i in configSetup.lang){
				let option = document.createElement("option");
				option.value = configSetup.lang[i];
				option.text = configSetup.lang[i];
				selectList.appendChild(option);
			}
			ver[0] = configSetup.ver[0];
			ver[1] = configSetup.ver[1];			
			document.getElementById("text_t1").innerText=' FW ver: ' + ver[0];
			document.getElementById("text_t6").innerText=' FS ver: ' + ver[1];
			timeZone = configSetup.input[3][2];
			selectList.value = configSetup.defaultLang;
			//document.getElementById("input00").value = configSetup.input[0][0]; //Количество дней в графике
			openlang(selectList.value);
			for (let i = 0; i <= 2; i++) { //Режимы реле Авто Вкл Выкл
				s_mode[i] = configSetup.s_mode[i];
				document.getElementById('select_smode'+(parseInt(i))).value = s_mode[i];	
			}    
			for (let i = 0; i <= 3; i++) { //Все инпуты
				for (let j in configSetup.input[i]) { 
					document.getElementById('input'+(parseInt(i))+(parseInt(j))).value = configSetup.input[i][j]; 
				}
			}
			document.getElementById('input111').checked = parseInt(document.getElementById('input110').value);  
			for (let i = 0; i <= 5; i++) {
				document.getElementById('sch_title'+(parseInt(i))).value = sch_title[selIndex][i] 
			}
			for (let j = 0; j <= 3; j++) {
				for (let i = 0; i <= 6; i++) { 																							 
					document.getElementById('schedule'+(parseInt(i))+(parseInt(j))).value = configSetup.schedule[selIndex][i][j];
				}
			}
			for (let i = 0; i <= 5; ++i) {
				document.getElementById('sch_title'+(parseInt(i))).value = sch_title[selIndex][i];
			}
			for (let j = 0; j <= 3; j++) {
				for (let i = 0; i <= 6; i++) {
					document.getElementById('schedule'+(parseInt(i))+(parseInt(j))).value = configSetup.schedule[selIndex][i][j];
				}
				ttip_rel[j] = configSetup.schedule[1][daysOfw[parsed_week]][j];
			}
			let ttip_rel1 = 'ON : ' + ttip_rel[0] + ' OFF: ' + ttip_rel[1];
			let ttip_rel2 = 'ON : ' + ttip_rel[2] + ' OFF: ' + ttip_rel[3];
			let ttip_temp = 'COR : '+configSetup.input[1][0]+' °C';
			let ttip_led = 'MAX: '+configSetup.input[1][1]+' % MIN : '+configSetup.input[1][2]+' %';
			let fan_on  = configSetup.input[1][3] + (configSetup.input[1][4] / 2);
			let fan_off = configSetup.input[1][3] - (configSetup.input[1][4] / 2);
			let ten_off  = configSetup.input[1][6] + (configSetup.input[1][7] / 2);
			let ten_on = configSetup.input[1][6] - (configSetup.input[1][7] / 2);
			let ttip_fan = 'ON : '+ fan_on +' °C OFF: '+ fan_off +' °C';
			let ttip_ten = 'OFF: '+ ten_off +' °C ON : '+ ten_on +' °C';
			document.getElementById("Picture6").setAttribute('tooltip_rel1', ttip_rel1);
			document.getElementById("Picture7").setAttribute('tooltip_rel2', ttip_rel2);	
			document.getElementById("Picture1").setAttribute('tooltip_temp', ttip_temp);
			document.getElementById("Picture3").setAttribute('tooltip_led', ttip_led);
			document.getElementById("Picture4").setAttribute('tooltip_fan', ttip_fan);
			document.getElementById("Picture5").setAttribute('tooltip_ten', ttip_ten);
			load_values = 1;
		}
	};
xhttp.send();
}
function ButtonClick(buttonNumb, method, url, setText, setBody) {
    let message = new Object();	
	message.loading = but[0];
	message.success = but[1];
	message.failure = but[2];
	let xhttp = new XMLHttpRequest();
	xhttp.onerror = function() {
		console.log("ERROR " + this.status);
	};
	xhttp.onload = function () {
		if (this.readyState == 4 && this.status == 200) {
			console.log(xhttp.response);	
			buttonNumb.value = message.success;
			//buttonNumb.value = xhttp.response;
			buttonNumb.setAttribute("class", "btn btn-success");	
			setTimeout(() => { 
				buttonNumb.value = setText;
				buttonNumb.setAttribute("class", "btn btn-primary");			
			}, 1000);
		} else if (this.readyState < 4){
			buttonNumb.value = message.loading;
			buttonNumb.setAttribute("class", "btn btn-warning");	
		} else {
    		console.log(xhttp.response);				
			buttonNumb.value = message.failure;
			//buttonNumb.value = xhttp.response;
			buttonNumb.setAttribute("class", "btn btn-danger");	
			setTimeout(() => { 
				buttonNumb.setAttribute("class", "btn btn-primary");
				buttonNumb.value = setText;
			}, 1000);
		}
	};
	xhttp.open(method, url, true);
    xhttp.timeout = 10000;
	xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    method == "GET"?xhttp.send():xhttp.send(setBody);
}
function init(){
	let button1 = document.getElementById("Button1");
	let button2 = document.getElementById("Button2");
	let button3 = document.getElementById("Button3");
	let button4 = document.getElementById("Button4");
	let button5 = document.getElementById("Button5");
	let button6 = document.getElementById("Button6");   
	let button7 = document.getElementById("Button7");
	let button8 = document.getElementById("Button8");
	let button9 = document.getElementById("Button9");
	let button10 = document.getElementById("Button10");
	let button11 = document.getElementById("Button11");	
	button1.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let jsonData = 'input[0][0]=' + document.getElementById("input00").value;
		ButtonClick(button1, "GET", 'chartDays?'+jsonData, but[5], ""); 
	});
	button2.addEventListener("click", function(event){ 
		let sched = [[],[],[],[],[],[],[],[]];
		let selIndex = document.getElementById("select_schedule").options.selectedIndex;
		let jsonData = '';
		event.preventDefault(); 
		for (let j = 0; j <= 3; j++) {
			for (let i = 0; i <= 6; i++) {
				sched[i][j] = document.getElementById('schedule'+(parseInt(i))+(parseInt(j))).value;
				jsonData += 'schedule['+selIndex+']['+i+']['+j+']='+sched[i][j]+'&';
			}
			if (selIndex) ttip_rel[j] = document.getElementById('schedule'+(parseInt(daysOfw[parsed_week]))+(parseInt(j))).value;
		}
		if (selIndex) {
			let ttip_rel1 = 'ON : ' + ttip_rel[0] + ' OFF: ' + ttip_rel[1];
			document.getElementById("Picture6").setAttribute('tooltip_rel1', ttip_rel1);
			let ttip_rel2 = 'ON : ' + ttip_rel[2] + ' OFF: ' + ttip_rel[3];
			document.getElementById("Picture7").setAttribute('tooltip_rel2', ttip_rel2);
		}
		ButtonClick(button2, "GET", 'save_schedule?selIndex='+selIndex+'&'+jsonData.substring(0, jsonData.length-1), but[5], ""); 
	});
	button3.addEventListener("click", function(event){ 
	 	let selIndex0 = document.getElementById("select_smode0").options.selectedIndex;
	 	let selIndex1 = document.getElementById("select_smode1").options.selectedIndex;
	 	let selIndex2 = document.getElementById("select_smode2").options.selectedIndex;		
		let jsonData = '';
		event.preventDefault(); 
 		for (let i = 0; i <= 2; i++) {
			s_mode[i] = document.getElementById("select_smode"+(parseInt(i))).options.selectedIndex;
			jsonData += 's_mode['+(parseInt(i))+']='+s_mode[i]+'&';
		} 
		ButtonClick(button3, "GET", 's_mode?'+jsonData.substring(0, jsonData.length-1), but[5], ""); 
	});	
	button4.addEventListener("click", function(event){ 
        let chk = (document.getElementById("input111").checked)?1:0; 
		document.getElementById("input110").value = chk;
		let jsonData = '';
		event.preventDefault(); 
		for (let i = 0; i <= 10; i++) {
			jsonData += 'input[1]['+i+']='+document.getElementById('input1'+(parseInt(i))).value+'&';
		}
		let fan_on  = parseInt(document.getElementById("input13").value) + (parseInt(document.getElementById("input14").value) / 2);
		let fan_off = parseInt(document.getElementById("input13").value) - (parseInt(document.getElementById("input14").value) / 2);
		let ten_off = parseInt(document.getElementById("input16").value) + (parseInt(document.getElementById("input17").value) / 2);
		let ten_on  = parseInt(document.getElementById("input16").value) - (parseInt(document.getElementById("input17").value) / 2);
		let ttip_fan = 'ON : '+ fan_on +' °C OFF: '+ fan_off +' °C';
		let ttip_ten = 'OFF: '+ ten_off +' °C ON : '+ ten_on +' °C';
		let ttip_temp = 'COR : '+document.getElementById('input10').value+' °C';	
		
		document.getElementById("Picture4").setAttribute('tooltip_fan', ttip_fan);
		document.getElementById("Picture5").setAttribute('tooltip_ten', ttip_ten);
		document.getElementById("Picture1").setAttribute('tooltip_temp', ttip_temp);		
		ButtonClick(button4, "GET", 'save?'+jsonData.substring(0, jsonData.length-1), but[5], ""); 
	});	
	button5.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
		for (let i = 0; i <= 12; i++) {
			jsonData += 'input[2]['+i+']='+document.getElementById('input2'+(parseInt(i))).value+'&';
		}
		ButtonClick(button5, "GET", 'mqtt?'+jsonData.substring(0, jsonData.length-1), but[5], ""); 
	});	
	button6.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let jsonData = 'input[3][0]=' + document.getElementById("input30").value + '&input[3][1]=' + document.getElementById("input31").value;
		ButtonClick(button6, "GET", 'save_date?'+jsonData, but[3], ""); 
	});
	button7.addEventListener("click", function(event){ 
		event.preventDefault(); 
		let jsonData = 'input[3][2]=' + document.getElementById("input32").value;
		ButtonClick(button7, "GET", 'auto_sync?'+jsonData, but[4], ""); 
	});		
	button8.addEventListener("click", function(event){ 
		let jsonData = '';
		event.preventDefault(); 
  		for (let i = 3; i <= 9; i++) {
			jsonData += 'input[3]['+i+']='+document.getElementById('input3'+(parseInt(i))).value+'&';
		} 
		ButtonClick(button8, "POST", '/ssid', but[5], jsonData.substring(0, jsonData.length-1)); 
	});	
	button11.addEventListener("click", function(event){ 
		event.preventDefault(); 
        if (device==1) {
		    ButtonClick(button11, "POST", '/restart', but[8], 'device='+device); 		
			setTimeout(() => {window.location.reload(true);}, 10000);
		}
	});
}
/*
function upload(file){
		var xhttp = new XMLHttpRequest();
		xhttp.onerror = function() {
			console.log("ERROR " + this.status);
			document.getElementById("updateMessage").innerHTML = "[ERROR " + this.status + "] EMPTY RESPONSE . PLEASE REUPDATE...";					
			document.getElementById("updateMessage").hidden = false; 
		};
		xhttp.onload = function() {
			if (this.status == 200) {			
				console.log("--UPLOAD_FILE_END--");					
    			console.log(xhttp.response);	
				console.log("PLEASE WAIT FOR 20 SEC...");
				document.getElementById("progressId").innerHTML = "100%";		
       			document.getElementById("progressId").style.width = "100%";			
				document.getElementById("updateID").hidden = true; 	
				document.getElementById("updateMessage").hidden = false; 					
				let secundomer = 20;
				setInterval(() => {
					document.getElementById("updateMessage").innerHTML = xhttp.response + ". RESTARTING. PLEASE WAIT FOR " + secundomer + " SEC";	
					(!secundomer) ? window.location.reload(true) : secundomer -= 1;			
				}, 1000);

			} else {
				document.getElementById("updateMessage").innerHTML = xhttp.response + "PLEASE REUPDATE...";					
				document.getElementById("updateMessage").hidden = false; 
			}				
		};
		xhttp.open("POST", '/update', true);
		//xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		xhttp.send(file);
}
*/
let eventType = 0;	
document.addEventListener('touchstart', handleTouchStart, false);
document.addEventListener('touchmove', handleTouchMove, false);
document.addEventListener('touchend', handleTouchEnd, false);
let xDown = null;
let yDown = null;
let xDiff = null;
let yDiff = null;
let timeDown = null;
let startEl = null;
function handleTouchEnd(e) {
	if (document.getElementById("opacity_all").hidden) { 
		if (startEl !== e.target) return;
		let swipeThreshold = 200; /* расстояние касания default 10px */
		let swipeTimeout = 1000;  /* время касания default 1000ms    */
		let timeDiff = Date.now() - timeDown;
		if (Math.abs(xDiff) > Math.abs(yDiff)) { // most significant
			if (Math.abs(xDiff) > swipeThreshold && timeDiff < swipeTimeout) {
				if (xDiff > 0) {
					eventType ++;				
					if (eventType > 4) eventType = 0;
				}
				else {
					eventType --;	
					if (eventType < 0) eventType = 4;
				}
			}
			changeZindex(eventType);
			document.getElementById('tab_'+ eventType).checked = true;		
			if (eventType==0) {updateChart();}
		}		
		if (eventType !== '') {
			startEl.dispatchEvent(new CustomEvent(eventType, { bubbles: true, cancelable: true }));
		}
		xDown = null;
		yDown = null;
		timeDown = null;
	}
};
function handleTouchStart(e){if (e.target.getAttribute('data-swipe-ignore') === 'true') return;startEl = e.target;timeDown = Date.now();xDown = e.touches[0].clientX;yDown = e.touches[0].clientY;xDiff = 0;yDiff = 0;}
function handleTouchMove(e) {if (!xDown || !yDown) return;let xUp = e.touches[0].clientX;let yUp = e.touches[0].clientY;xDiff = xDown - xUp;yDiff = yDown - yUp;}