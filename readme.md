## Fredmill project

Yo! This repo consists of several parts in different folders.

- **teensy-sensor-hub** The code for the teensy doing sensor stuff
- **esp32-websocket-client** The code for the esp32 connecting to the cloud and using websockets for real-time communication.
- **webserver** the node server that handles websockets and serves the static pages. Run on heroku (or similar).
- **webserver/vue-frontend** The vue project for visualizing the sensor data. **_Important!_** `vue.config.js` is configured to build into the webservers public folder. Also some node modules that usually are _devDependencies_ are now placed as regular _dependencies_. This is to be able to build the vue project from within heroku.

## Heroku specifics

### package.json

Check out `package.json` in the webserver folder. This is the entry point for heroku. I've added a postinstall command that builds the vue project on the server. heroku calls postinstall first, then the start command.

### running repo subfolders on heroku

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
