# HttpServerPlugin

HttpServerPlugin for Unreal Engine 5 - a plugin that allows you to flexibly, quickly and easily configure a RESTful API server in an application on UE 5 using a config, to interact with it via http from the network. In the configs, you only need to define access points (endpoints), the request type (GET, POST etc.) and in the project code - redefine the response to these requests.

## How to use

The connection algorithm is as follows:

1. [recommendations for joining new projects](#1-recommendations-for-joining-new-projects)
2. [add to project dependency](#2-add-to-project-dependency);
3. [set up the config](#3-set-up-the-config);
4. [override request processing logic](#4-override-the-request-processing-logic):
   1. [in blueprint](#41-in-blueprint);
   2. [in code](#42-in-code);

Prof IT!

## 1. Recommendations for joining new projects

* Plugin repository: https://github.com/lpestl/HttpServerPlugin

The following methods are recommended for connecting to the project and reusing:

1. To connect to a new project, it is recommended to connect this plugin to the repository as a git submodule. To do this, in the repository root (in the folder where the `*.uproject` file is located) of the project to which you want to connect this plugin, you need to run the following command:

```bash
git submodule add https://github.com/lpestl/HttpServerPlugin.git Plugins/HttpServerPlugin
```

This command will download the plugin repository and connect it as a submodule to the current project. By default, the latest stable version from the main branch will be downloaded.

2. Download the latest stable version using one of the builds on the [Releases](https://github.com/lpestl/HttpServerPlugin/releases) page. Unzip the source archive to the `{project_path}/Plugins/` folder.


After that, it will be enough to regenerate the UE project and rebuild it in order to start using the connected plugin.

## 2. Add to project dependency

First, you need to make sure that the `HttpServerPlugin` is included in the project. To do this, open the project in Unreal Editor, select the menu item `Edit -> Plugins`, enter `HttpServer` in the search and make sure that the box opposite the `HttpServerPlugin` is checked.

![Enable plugin](media/02_check_plugin.png)

The next step in the `*.Build.cs` file is to add the `HttpServerWrapperModule` module to the list of dependencies. For example:

![Add dependency](media/03_add_dependency.png)

## 3. Set up the config

The next step is to configure the module config file. This can be done using the UI: run the project in Unreal Editor and select `Settings -> Project` Settings in the window that opens on the left, select the section called `Plugins -> HTTP Server Settings` in the right part of the window, fill in the settings. For example:

![REST Api Settings](media/04_restapi_settings.png)

After filling in the settings, the file `{repo_path}/Config/DefaultHttpServer.ini` appears with approximately the following content:

```ini
[/Script/HttpServer.HttpServerSettings]
Port=8080
+Endpoints=(Endpoint="/get/data",Verbs=VERB_GET)
+Endpoints=(Endpoint="/add/data",Verbs=VERB_POST)
+Endpoints=(Endpoint="/update/data",Verbs=VERB_PUT)
+Endpoints=(Endpoint="/delete/data",Verbs=VERB_DELETE)
```

## 4. Override the request processing logic

The main entity, which is a wrapper of the http server logic, is the `UHttpServer` class, which is inherited from `UObject`. To run a web server in your project, you can either create a blueprint and then create an object and use it, or create an object directly in the code.

### 4.1 In blueprint

* Creating and running the server:

![Create and run](media/05_create_and_run_server.png)

* Stopping the server:

![Stopping the server](media/06_stop_server.png)

* Example of request processing:

![Example](media/07_example_implement.png)

### 4.2 In code

Declare a pointer to `UHttpServer`, for example by making it a class member:

```C++
private:
    UPROPERTY(EditAnywhere)
    UHttpServer* HttpServer;
};
```

Create a new class object:

```C++
HttpServer = NewObject<UHttpServer>();
```

Subscribe to the delegate that will be called when a request is received via http:

```C++
    HttpServer->OnReceivedRequest().AddDynamic(this, &ASomeNewGameMode::OnReceivedRequest);
    //...
}
void ASomeNewGameMode::OnReceivedRequest(const FString& RequestBody, FEndpointData EndpointData)
{
    //...
}
```

And start the server:

```C++
HttpServer->StartApiServer();
```

In this case, you should not forget to stop the server in the appropriate place:

```C++
HttpServer->StopApiServer();
```

As an example:

```C++
#include "SomeNewGameMode.h"
#include "HttpServer.h"

void ASomeNewGameMode::BeginPlay()
{
    Super::BeginPlay();
    HttpServer = NewObject<UHttpServer>();
    HttpServer->OnReceivedRequest().AddDynamic(this, &ASomeNewGameMode::OnReceivedRequest);
    HttpServer->StartApiServer();
}

void ASomeNewGameMode::OnReceivedRequest(const FString& RequestBody, FEndpointData EndpointData)
{
    //...
}

void ASomeNewGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    HttpServer->StopApiServer();
    Super::EndPlay(EndPlayReason);
}

```

For full control over request processing and response sending, create your own class inherited from `UHttpServer` and override the virtual function:

```C++
virtual bool HandleRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete, FEndpointData EndpointData);
```