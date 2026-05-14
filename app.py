import os
import google.generativeai as genai
from flask import Flask, request, jsonify, render_template
from scheduler import *
from dotenv import load_dotenv
load_dotenv()
app = Flask(__name__)
API_KEY=os.environ.get("VITE_GEMINI_API_KEY")
genai.configure(api_key=API_KEY)

model = genai.GenerativeModel("gemini-1.5-flash")
@app.route('/')
def home():
    return render_template('index.html')


@app.route('/run', methods=['POST'])
def run_scheduler():
    data = request.json

    tasks_input = data["tasks"]
    algorithm = data["algorithm"]
    quantum = data.get("quantum", 2)

    tasks = []
    for i, t in enumerate(tasks_input):
        tasks.append(Task(i+1, t["arrival"], t["execution"], t["priority"], t["execution"]))

    # Choose strategy
    if algorithm == "FIFO":
        strategy = FIFO()
    elif algorithm == "SJF":
        strategy = SJF()
    elif algorithm == "PRIORITY":
        strategy = PriorityScheduling()
    elif algorithm == "RR":
        strategy = RoundRobin(quantum)
    else:
        return jsonify({"error": "Invalid algorithm"})

    scheduler = Scheduler(strategy)
    result = scheduler.execute(tasks)
    prompt = f"""
    You are an Operating Systems scheduling expert.

    Analyze the following scheduling metrics and recommend the best scheduling algorithm.

    Algorithm Used:
    {algorithm}

    Metrics:
    Average Waiting Time: {result['average_waiting_time']}
    Average Turnaround Time: {result['average_turnaround_time']}
    Average Response Time: {result['average_response_time']}

    Process Metrics:
    {result['metrics']}

    Explain:
    1. Whether this algorithm performed well
    2. Which scheduling algorithm may perform better
    3. Why
    4. Workload characteristics
    5. Fairness and responsiveness analysis

    Keep response concise and technical.
    """
    try:

        response = model.generate_content(prompt)

        ai_analysis = response.text

    except Exception as e:

        ai_analysis = f"AI analysis unavailable: {str(e)}"

    result["ai_analysis"] = ai_analysis
    return jsonify(result)



if __name__ == "__main__":
    app.run(debug=True)