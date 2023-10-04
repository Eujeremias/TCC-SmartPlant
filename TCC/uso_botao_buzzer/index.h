const char pagina[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8">
	</head>

	<body>

		<h1><center>Atualização de página Web<br>no NodeMCU sem <i>refresh</i><br>
        <hr><hr><br>
		Valor aleatório: <span id="valor">0</span></center></h1>

    <button type="button" class="bt" onclick="botao(1)">Ligar bomba</button><br>
    <button type="button" class="bt" onclick="botao(2)">Desligar bomba</button><br>
 
		<script>

    <!--botao-->
            function botao(x) {
            var xmlhttp = new XMLHttpRequest();
            xmlhttp.open("GET", "requisicaoBotao?nBotao=" + x, true);
            xmlhttp.send();
        }

        setInterval(function () { //setInterval(callback, delay)
            getData1();
        }, 1000); //atualização em milisegundos ou pode n definir o intervalo tb

        function getData1() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("horario").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "requisicaoHorario", true);
            xhttp.send();
        }

      <!--fim botao-->

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

			setInterval(function() {
				// Chama uma função repetidamente com intervalo de 1 segundo
				getData();
			}, 1000); //taxa de atualização: 1000 milisegundos
		</script>

	</body>
</html>
)=====";
