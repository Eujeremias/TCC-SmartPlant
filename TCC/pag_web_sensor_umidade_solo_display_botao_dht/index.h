const char pagina[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8">
	</head>

	<body>

		<h1><center>Informações sobre o tempo<br>
        <hr><br>
		Valor aleatório: <span id="valor">0</span></center></h1>
      <hr><br>
    <h1><center>Umidade do solo: <span id="umidade">0</span></center></h1>
    <h1><center>Umidade do Ar: <span id="umidadeTempo">0</span></center></h1>
    <h1><center>Temperatura: <span id ="temperatura">0</span></center></h1>
    <a href="https://www.instagram.com/jeremiasverissimogomes/"> Clique aqui</a>
 
		<script>
			function getData() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("valor").innerHTML = this.responseText;
					}
				};
				xhttp.open("GET", "lerNUM", true);
				xhttp.send();
			}
      
      function umidade() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("umidade").innerHTML = this.responseText;
					}
				};
				xhttp.open("GET", "lerUmidade", true);
				xhttp.send();
			}

      function umidadeTempo() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("umidadeTempo").innerHTML = this.responseText;
					}
				};
				xhttp.open("GET", "lerUmidadeTempo", true);
				xhttp.send();
			}

      function temperatura() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("temperatura").innerHTML = this.responseText;
					}
				};
				xhttp.open("GET", "lerTemperatura", true);
				xhttp.send();
			}

			setInterval(function() {
				// Chama uma função repetidamente com intervalo de 1 segundo
				getData();
        umidade();
        umidadeTempo();
        temperatura();
			}, 1000); //taxa de atualização: 1000 milisegundos

		</script>

	</body>
</html>
)=====";
