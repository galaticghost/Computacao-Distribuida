from fastapi import FastAPI
from contextlib import asynccontextmanager
from pydantic import BaseModel
from dotenv import load_dotenv
from redis.asyncio import Redis
import aio_pika
import os
import json

load_dotenv()

CLOUDAMQP_URL = os.getenv("CLOUDAMQP_URL")
REDIS_HOST = os.getenv("REDIS_HOST")
REDIS_PASS = os.getenv("REDIS_PASS")
connection = None
channel = None
exchange = None
redis = None

class Telemetria(BaseModel):
    temperatura: float
    umidade: float

@asynccontextmanager
async def lifespan(app:FastAPI):
    global connection, channel, exchange, redis

    redis = Redis(
        host=REDIS_HOST,
        port=15899,
        password=REDIS_PASS,
        decode_responses=True
    )

    connection = await aio_pika.connect_robust(CLOUDAMQP_URL)
    channel = await connection.channel()

    exchange = await channel.get_exchange(
        "sensors_actuators_exchange"
    )

    queue = await channel.declare_queue(
        "fila.temperatura",
        passive=True
    )

    await queue.bind(
        exchange,
        routing_key="sistema1.sensor.temperatura"
    )

    print("Conectado ao LavinMQ")
    
    await queue.consume(callback)

    yield

    await connection.close()
    await redis.close()

app = FastAPI(lifespan=lifespan)

@app.post("/telemetria")
async def read_data(dados: Telemetria):
    print(f"temp: {dados.temperatura}")
    print(f"Umidade: {dados.umidade}")

    payload = dados.model_dump_json().encode()

    await exchange.publish(
        aio_pika.Message(
            body=payload,
            content_type="application/json"
        ),
        routing_key="sistema1.sensor.temperatura"
    )

    return {"status": "ok"}

async def callback(message: aio_pika.IncomingMessage):
    async with message.process():
        dados = json.loads(
            message.body.decode()
        )

        temperatura = dados["temperatura"]
        umidade = dados["umidade"]

        await redis.set(
            "sistema1:temperatura",
            temperatura
        )

        await redis.incr("total_leituras_turma")
