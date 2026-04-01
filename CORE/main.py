
from fastapi import FastAPI
from pydantic import BaseModel
from google import genai
import requests
import traceback

# 🔐 KEY (dán key Gemini của bro)
GEMINI_API_KEY = "AIzaSyBPqwHlcS7fqlA8XsodSmf6L8qFxFWXcNM"

# OpenRouter fallback (optional – có thì thêm)
OPENROUTER_API_KEY = "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"

client = genai.Client(api_key=GEMINI_API_KEY)

GEMINI_MODEL = "gemini-1.5-flash"  # dùng 1.5 ổn định hơn
OPENROUTER_MODEL = "openrouter/free"

app = FastAPI()

class Prompt(BaseModel):
    text: str

@app.get("/")
def home():
    return {"status": "AI server ok 😎"}

@app.post("/ask")
def ask_ai(prompt: Prompt):
    try:
        print("📩 INPUT:", prompt.text)

        # ===== 1. THỬ GEMINI =====
        try:
            response = client.models.generate_content(
                model=GEMINI_MODEL,
                contents=prompt.text
            )

            if response.text:
                return {"result": response.text}

        except Exception as e:
            print("⚠️ Gemini lỗi:", e)

        # ===== 2. FALLBACK OPENROUTER =====
        try:
            headers = {
                "Authorization": f"Bearer {OPENROUTER_API_KEY}",
                "Content-Type": "application/json"
            }

            data = {
                "model": OPENROUTER_MODEL,
                "messages": [
                    {"role": "user", "content": prompt.text}
                ]
            }

            r = requests.post(
                "https://openrouter.ai/api/v1/chat/completions",
                headers=headers,
                json=data,
                timeout=20
            )

            result = r.json()["choices"][0]["message"]["content"]

            return {"result": result}

        except Exception as e:
            print("❌ Fallback lỗi:", e)
            return {"error": "AI đang bận, thử lại sau 😭"}

    except Exception as e:
        traceback.print_exc()
        return {"error": str(e)}