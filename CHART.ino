void update_chart_values() {
  int ostatok = ds_hour % 2;
  if (ostatok == 0 && flag == 0 && temp_filtered != 0.00) {
    int point = ds_hour / 2;
    jsonWrite(configChart, "days", ds_day, point, temp_filtered);
    saveConfigChart();
    flag = 1;
  } else if (ostatok != 0) {
    flag = 0;
  }
}
