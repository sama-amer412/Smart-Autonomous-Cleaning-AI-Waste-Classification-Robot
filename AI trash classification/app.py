import streamlit as st
import numpy as np
import cv2
from tensorflow.keras.models import load_model
from PIL import Image
from streamlit_webrtc import webrtc_streamer, VideoProcessorBase
import av
import paho.mqtt.client as mqtt

# =========================
# MQTT
# =========================

BROKER = "insert URL"
PORT = "port number"
USERNAME = "username"
PASSWORD = "password"

mqtt_client = mqtt.Client()

mqtt_client.username_pw_set(USERNAME, PASSWORD)

mqtt_client.tls_set()

mqtt_client.connect(BROKER, PORT)

mqtt_client.loop_start()

# =========================
# MODEL
# =========================

model = load_model("recycle_model_clean.keras")

class_names = [
    'glass',
    'metal',
    'paper',
    'plastic',
    'shoes'
]

# =========================
# PREDICT
# =========================

def predict(img):

    img_resized = cv2.resize(img, (224, 224))

    img_input = np.expand_dims(img_resized, axis=0)

    pred = model.predict(img_input, verbose=0)[0]

    class_index = np.argmax(pred)

    confidence = float(np.max(pred))

    label = class_names[class_index]

    probs = {
        class_names[i]: float(pred[i])
        for i in range(len(class_names))
    }

    probs = dict(
        sorted(
            probs.items(),
            key=lambda x: x[1],
            reverse=True
        )
    )

    if label == "plastic":
        msg = "⚠️ Plastic detected"

    elif label == "glass":
        msg = "🍾 Glass detected"

    elif label == "metal":
        msg = "🔩 Metal detected"

    elif label == "paper":
        msg = "📄 Paper detected"

    else:
        msg = "👟 Shoes / Other detected"

    return label, confidence, msg, probs


# =========================
# CAMERA CLASS
# =========================

class VideoProcessor(VideoProcessorBase):

    def __init__(self):

        self.frame_count = 0
        self.last_label = ""
        self.last_conf = 0.0

    def recv(self, frame):

        img = frame.to_ndarray(format="bgr24")

        self.frame_count += 1

        if self.frame_count % 10 == 0:

            label, conf, msg, _ = predict(img)

            self.last_label = f"{label} ({conf:.2f})"

            self.last_conf = conf

        cv2.putText(
            img,
            self.last_label,
            (10, 30),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (0, 255, 0),
            2
        )

        return av.VideoFrame.from_ndarray(
            img,
            format="bgr24"
        )


# =========================
# UI
# =========================

st.set_page_config(
    page_title="Recycle Classifier",
    layout="centered"
)

st.title("♻️ Recycle Waste Classifier")

tab1, tab2 = st.tabs([
    "📷 Upload",
    "🎥 Live Camera"
])

# =========================
# UPLOAD
# =========================

with tab1:

    files = st.file_uploader(
        "Upload up to 4 images",
        type=["jpg", "png", "jpeg"],
        accept_multiple_files=True
    )

    if files:

        for file in files[:4]:

            file_bytes = np.asarray(
                bytearray(file.read()),
                dtype=np.uint8
            )

            img = cv2.imdecode(file_bytes, 1)

            st.image(img, width=200)

            label, confidence, msg, probs = predict(img)

            st.success(f"Prediction: {label}")

            st.info(f"Confidence: {confidence:.2f}")

            st.write(msg)

            st.write("### 📊 Probabilities")

            for k, v in probs.items():

                st.write(f"{k}: {v:.2f}")

            # =========================
            # SEND TO CLOUD BUTTON
            # =========================

            if st.button(
                f"Send Result To Cloud ({label})",
                key=file.name
            ):

                mqtt_client.publish(
                    "material",
                    label
                )

                st.success(
                    f"{label} sent to cloud successfully"
                )

            st.write("---")


# =========================
# LIVE CAMERA
# =========================

with tab2:

    webrtc_streamer(
        key="live",
        video_processor_factory=VideoProcessor,
        async_processing=True,
        media_stream_constraints={
            "video": {
                "width": 320,
                "height": 240,
                "frameRate": 15
            },
            "audio": False,
        },
    )