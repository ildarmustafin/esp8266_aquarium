static const char successResponse[] PROGMEM =
  "<META http-equiv=\"refresh\" content=\"20;URL=/\">Update Success! Rebooting...Wait for 20 sec\n";

static const char upload_spiffs[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<HTML>
  <HEAD>
    <TITLE>ESP_UPDATE</TITLE>
  </HEAD>
  <BODY>
    <form id="update" method="POST" action="update" enctype="multipart/form-data"> 
      <input type="file" name="Update">
      <input type="submit" value="Update">        
    </form> 
  </BODY>
</HTML>        
)rawliteral";

const char debug_page[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<HTML>
  <HEAD>
    <META http-equiv="refresh" content="1;URL=/info">
    <TITLE>ESP_INFO</TITLE>
  </HEAD>
  <BODY>
    <label>1. Chip ID: %GETCHIPID%</label><br/>      
    <label>2. Flash Chip ID: %GETFLASHCHIPID%</label><br/>
    <label>3. Core ver: %GETCOREVER%</label><br/>          
    <label>4. Boot ver: %GETBOOTVER%</label><br/>  
    <label>5. CPU freq: %GETCPUFREQMHZ% MHz</label><br/>
    <label>6. Flash Chip Speed: %GETFLASHCHIPSPEED% MHz</label><br/>  
    <label>7. SDK ver: %GETSDKVER%</label><br/>    
    <label>8. Sketch MD5: %GETSKETCHMD5%</label><br/>        
    <label>9. Flash Chip Real Size: %GETFLAHCHIPREALSIZE% bytes</label><br/>
    <label>10. Flash Chip Size: %GETFLAHCHIPSIZE% bytes</label><br/>    
    <label>11. Free Sketch Space: %GETFREESKETCHSPACE% bytes</label><br/>
    <label>12. Sketch Size: %GETSKETCHSIZE% bytes</label><br/>
    <label>13. Cycle count: %GETCYCLECOUNT%</label><br/>  
    <label>14. FreeHeap: %GETFREEHEAP% bytes</label><br/>    
  </BODY>
</HTML>
)rawliteral";
