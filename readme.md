## Threadmill project

Yo! This repo consists of several parts in different folders.

- **teensy-sensor-hub** The code for the teensy doing sensor stuff
- **esp32-websocket-client** The code for the esp32 connecting to the cloud and using websockets for real-time communication.
- **webserver** the node server that handles websockets and serves the static pages. Run on heroku (or similar).
- **threadmill-frontend** The vue project for visualizing the sensor data. **_Important!_** Configured (`vue.config.js`) to build into the webservers public folder.

## Heroku specifics

Because the server code is in a subfolder of the repo we use git subtree to deploy to heroku.
The --prefix **webserver** is the name of the subfolder that should be pushed to heroku.

```
git push heroku `git subtree split --prefix webserver master`:master --force
```

---

This should also work, but git might complain about being behind upstream. See [this](https://stackoverflow.com/questions/13756055/git-subtree-subtree-up-to-date-but-cant-push/15623469#15623469) stack overflow question.

```
git subtree push --prefix webserver heroku master
```
