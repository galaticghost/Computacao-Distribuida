# Computacao-Distribuida
Projeto final da disciplina de Computação Distribuída.

# Fluxo do Servidor
A aplicação utiliza programação assíncrona baseada em asyncio.

Durante a inicialização do servidor, a função lifespan é executada. Nela são criadas as conexões persistentes com um banco Redis e um broker LavinMQ.

Após a inicialização:
1. O servidor fica aguardando requisições POST enviadas pelo arduino.
2. Ao receber uma requisição, o servidor serializa os dados em um json e os publica na exchange
sensors_actuators_exchange.
3. A exchange roteia a mensagem para a fila fila_temperatura.
4. O consumidor AMQP permanece escutando a fila de temperatura.
5. Quando uma mensagem chega, a função callback é executada.
6. Utilizando os dados recebidos ele envia a temperatura para o Redis com a função SET e incrementa o contador com a função INCR.

Todo o fluxo é executado de forma assíncrona utilizando async e await, permitindo que múltiplas operações de rede ocorram concorrentemente sem bloquear a aplicação.
# Como Rodar o Projeto
1. Crie o ambiente virtual. 
```bash
    python3 -m venv .venv
```

2. Execute o ambiente.
```bash
   source .venv/bin/activate
```

3. Instale as dependências.
```bash
   pip install -r requirements.txt
```

4. Defina as variáveis no arquivo env.
```env
    REDIS_HOST=seu_host
    REDIS_PASSWORD=sua_senha
    CLOUDAMQP_URL=url_de_seu_amqp
```

5. Rode o servidor.
```bash
    fastapi run
```