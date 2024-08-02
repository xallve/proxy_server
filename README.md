# proxy_server
Proxy server written in C to detect XSS and SQL injections

## Overview

This project implements a proxy server that intercepts requests, performs security checks, and validates potentially harmful requests by executing them in a sandbox environment. It is designed to be deployed on top of any existing server, adding an additional layer of security.

## Features

* Request Interception: Captures incoming requests and performs preliminary security checks.
* Sandbox Execution: Validates potentially harmful requests by executing them in a controlled sandbox environment (Remote server with backup data working as mediator).
* Flexibility: Allows the main server to define the logic for sandbox validation.
* Cross-Language Integration: Main server and proxy server can be written in different languages.

## Components

* Proxy Server: Written in C, intercepts requests, performs preliminary checks, and forwards requests to the main server for sandbox validation.
* Sandbox Server: Written in Python using Flask, receives requests from the proxy server, executes them in a sandbox, and returns the validation results.

## Sandbox example

```python
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/sandbox', methods=['POST'])
def sandbox():
    req_data = request.get_json()
    request_body = req_data['request']

    # Example validation logic (replace with your own)
    if "DROP" in request_body:
        return jsonify({"status": "harmful"})
    else:
        return jsonify({"status": "safe"})

if __name__ == '__main__':
    app.run(port=5000)
```
