<div align="center">

# 🤖 ESP32-MicroBot

### A tiny desk robot with a *soul* — animated AI eyes, real physics, and a Telegram brain. All on a $5 chip.

**No 3D printer. No PCB. No soldering skills required. Just an ESP32-S3, an OLED, and 15 minutes.**

[![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Ready-e7352c?logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32-s3)
[![Arduino](https://img.shields.io/badge/Arduino-IDE-00979D?logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Telegram](https://img.shields.io/badge/Telegram-Bot%20Control-26A5E4?logo=telegram&logoColor=white)](https://core.telegram.org/bots)
[![AI](https://img.shields.io/badge/AI-OpenAI%20Compatible-black?logo=openai)](https://platform.openai.com/docs/api-reference/chat)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](#-contributing)

*It blinks. It yawns. It falls asleep when you ignore it. It gets sad when it rains.*

**⭐ Star this repo if your desk deserves a pet that never needs feeding.**

</div>

---

## 👀 What is this?

MicroBot turns a bare **ESP32-S3 + 0.96" OLED** into an expressive desktop companion you control entirely from **Telegram** — from anywhere on Earth.

It's not a static face slapped on a screen. Every eye movement runs through a **spring-damper physics engine**: eyes overshoot, settle, and breathe. Pupils lag behind the eyeballs like they have real mass. It saccades around the room, blinks on natural random intervals, and when you leave it alone long enough… it stretches, yawns, and drifts off to sleep with floating `Zzz`.

Then a Telegram message comes in and it snaps awake.

## ✨ Features

| | |
|---|---|
| 🎭 **18 moods** | happy, sad, angry, love, evil, suspicious, cry (with animated tears!), laugh, shock, wink, cool, tired, focus, confused, excited, sleepy, surprised, normal |
| 🧠 **Ask AI anything** | `/ask why is the sky blue` → routes to any **OpenAI-compatible API** (OpenAI, Groq, Ollama, OpenRouter…) and replies in Telegram while the bot shows "Thinking…" |
| 🌦️ **Live weather + mood sync** | OpenWeatherMap with animated rain, snow, lightning flashes, drifting clouds, fog — and the bot's *mood follows the weather*. Rain = sad eyes 🥺 |
| 📅 **3-day forecast** | Clean 3-column card with mini weather icons |
| ⏰ **Clock + world clock** | NTP-synced big digital clock, dual-timezone world view |
| 🔔 **Reminders** | `/remind take a break 25m` — buzzes, pops up on screen, *and* pings you back on Telegram |
| 📢 **Notifications API** | `curl "http://<bot-ip>/notify?title=Build&msg=Deploy%20done"` — pipe CI alerts, doorbells, anything onto its face |
| 📱 **Zero-code setup portal** | First boot spins up a WiFi hotspot with a slick dark-mode config page. No re-flashing to change WiFi, keys, or city |
| 🔊 **Smart buzzer** | Auto-detects active-high / active-low / passive buzzers at boot. Just wire it and go |
| 💤 **Idle life** | 13 autonomous idle behaviors — looking around, double blinks, stretching, yawning, sleeping, scanning the room |
| ⚡ **Dual-core FreeRTOS** | Telegram polling pinned to core 0, smooth animation on core 1. Mutexes, queues, the works — no janky frame drops while fetching messages |

## 🎬 The physics engine (the secret sauce)

Most OLED "robot eyes" projects lerp between keyframes. MicroBot simulates:

```cpp
// Every frame, for every eye:
velX = (velX + (targetX - x) * k) * d;   // spring toward target, damped
x += velX;                                // eyes have momentum

// Pupils use SOFTER springs than eyeballs →
// they drag behind and settle late, like real eyes with real mass
pVelX = (pVelX + (targetPupilX - pupilX) * pk) * pd;
```

Add a sine-wave **breathing effect**, randomized **saccades** with head-follow, per-mood **blink timing** (a shocked bot blinks fast; a cool bot barely blinks), and eyelid masks cut from triangles — and you get something that feels *alive* on a 128×64 monochrome panel.

## 🛒 Bill of materials (~$8)

| Part | Notes |
|---|---|
| ESP32-S3 dev board | Any ESP32 works with pin tweaks |
| SSD1306 OLED 128×64 (I²C) | The classic 0.96" |
| Buzzer (optional) | Active or passive — auto-detected |
| 4 jumper wires | That's it. That's the build. |

## 🔌 Wiring

```
OLED SDA  →  GPIO 8
OLED SCL  →  GPIO 9
OLED VCC  →  3V3
OLED GND  →  GND
Buzzer +  →  GPIO 4   (optional)
Buzzer –  →  GND
```

## 🚀 Quick start

**1. Install libraries** (Arduino IDE → Library Manager):

- `Adafruit SSD1306` + `Adafruit GFX`
- `UniversalTelegramBot`
- `ArduinoJson` + `Arduino_JSON`

**2. Create your Telegram bot**

Message [@BotFather](https://t.me/BotFather) → `/newbot` → copy the token.

**3. Get a free weather key**

Sign up at [openweathermap.org](https://openweathermap.org/api) (free tier is plenty).

**4. Flash it**

Open `microbot/microbot.ino`, select your ESP32-S3 board, upload.

**5. Configure over WiFi — no code edits needed**

On first boot (or if WiFi fails), MicroBot becomes a hotspot:

```
📶 Connect to WiFi:  ESP32-MicroBot   (password: 123456789)
🌐 Open:             http://192.168.4.1
```

Fill in WiFi, weather key, city, timezone, Telegram token, and (optionally) an AI endpoint. Hit **Save & Reboot**. Done — your robot is alive.

> ⚠️ **Heads up:** replace the placeholder fallback credentials in `loadConfig()` with your own (or leave them blank and use the portal). Never commit real keys.

### ⚙️ MacroDroid Setup

#### Trigger

```
Notification Received
```

Choose:

```
Any Application
```

or select only the apps you want.

---

#### Action

```
HTTP Request (GET)
```

URL:

```text
http://192.168.1.131/notify?title={not_app_name}&msg={not_title}
```

Replace:

```
192.168.1.131
```

with your own MicroBot IP.

---

### Available Variables

| Variable | Description |
|-----------|-------------|
| `{not_app_name}` | App name (WhatsApp, Telegram, Gmail...) |
| `{not_title}` | Notification title |
| `{not_text}` | Notification content (optional) |

For full notification content:

```text
http://192.168.1.131/notify?title={not_app_name}&msg={not_text}
```

or combine both:

```text
http://192.168.1.131/notify?title={not_app_name}&msg={not_title}%0A{not_text}
```


## 💬 Telegram commands

| Command | What happens |
|---|---|
| `/face` | Animated eyes (the main event) |
| `/mood evil` | Any of 18 moods — try `cry`, `suspicious`, `cool` |
| `/ask <question>` | Ask the AI — bot shows *Thinking…* on screen |
| `/weather` | Animated live weather card |
| `/forecast` | 3-day forecast |
| `/clock` / `/world` | Big clock / world clock |
| `/notify Lunch \| Pizza is here` | Full-screen alert + beep |
| `/remind stretch 45m` | Reminder → buzz + popup + Telegram ping |
| `/reminders` / `/cancelreminder 3` | Manage active reminders |
| `/config` | Get the bot's IP for the settings page |

## 🧩 Make it yours

- **New mood?** Add a `MOOD_*` constant, an eyelid mask in `drawEyelidMask()`, and a shape in `updatePhysicsAndMood()`. Three functions, one new personality.
- **New idle behavior?** Add to the `IdleAction` enum and `applyIdleActionTargets()`.
- **Local AI?** Point the AI endpoint at your own [Ollama](https://ollama.com) box — fully offline brain.
- **Home Assistant / n8n / CI?** Hit the `/notify` HTTP endpoint from anywhere on your LAN.

## 🗺️ Roadmap

- [ ] Touch/tap interaction (petting → love mood 💕)
- [ ] Sound-reactive eyes
- [ ] Configurable world-clock cities
- [ ] AI-driven mood (let the LLM pick how it feels about your question)
- [ ] 3D-printable desk shell

## 🤝 Contributing

Found a bug? Built a new mood? Made it dance? PRs and issues are very welcome — this project is meant to be forked, remixed, and stuck to the top of monitors worldwide.

## 📜 License

[MIT](LICENSE) © 2026 **Shriful Islam (InHumanZ)**

---

<div align="center">

**If MicroBot made you smile, [⭐ star the repo](../../stargazers) — it makes the eyes go `MOOD_LOVE`.**

*Built with ❤️, springs, dampers, and one very patient OLED.*

</div>
