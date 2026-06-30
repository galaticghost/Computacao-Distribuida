import time
import random
import requests

URL = f"http://192.168.31.128:8000/telemetria"

def gerar_dados_simulados():
    return round(random.uniform(20,23),1), random.randint(50,60)


def enviar_dados(temperatura,umidade):
    """Imprime e envia a distância gerada para o PC Gateway."""
    print(f"Temperatura: {temperatura}")
    print(f"Umidade: {umidade}")

    dados = {"temperatura": temperatura, "umidade": umidade}

    try:
        resposta = requests.post(URL, json=dados, timeout=3)
        if resposta.status_code == 200:
            print(f"Sucesso")
        else:
            print(f"Status: {resposta.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Não foi possível conectar ao PC Gateway: {e}")

def loop_emulador():
    ultima_leitura_sensor = 0
    intervalo_sensor = 5

    while True:
        tempo_agora = time.time()

        if tempo_agora - ultima_leitura_sensor >= intervalo_sensor:
            temperatura, umidade = gerar_dados_simulados()
            enviar_dados(temperatura,umidade)
            ultima_leitura_sensor = tempo_agora

        time.sleep(intervalo_sensor)


if __name__ == "__main__":
    loop_emulador()