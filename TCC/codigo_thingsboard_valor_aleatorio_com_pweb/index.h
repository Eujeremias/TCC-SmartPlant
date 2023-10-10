const char pagina[] PROGMEM = R"=====(
<!DOCTYPE html>

<html>
  <head>
    <meta charset="UTF-8">
  </head>

  <body>
    <h1><center>Atualização de página Web<br> no NodeMCU sem <i>refresh</i><br>
    <hr><br>
    Valor Aleatório: <span id = "valor">0</span></center></h1>

    <script>
      function getData(){
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function(){
          if(this.readyState == 4 && this.status == 200){
            document.getElementById("valor").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "lerNUM", true);
        xhttp.send();
      }
      setInterval(function(){
        getData();
      }, 1000);
    </script>
  </body>

</html>
)=====";