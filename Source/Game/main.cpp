#include "Game/Application/Application.h"
#include "Game/Application/Message.h"
#include "Game/Application/ConsoleCommandHandler.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/DebugHandler.h>

#include <future>

i32 main()
{
	Application app;
	app.Start(true);

    ConsoleCommandHandler commandHandler;
    auto future = std::async(std::launch::async, StringUtils::GetLineFromCin);
    while (true)
    {
        bool shouldExit = false;

        MessageOutbound message;
        while (app.TryGetMessageOutbound(message))
        {
            assert(message.type != MessageOutbound::Type::Invalid);

            switch (message.type)
            {
                case MessageOutbound::Type::Print:
                {
                    DebugHandler::Print(message.data);
                    break;
                }

                case MessageOutbound::Type::Pong:
                {
                    DebugHandler::Print("Application Thread -> Main Thread : Pong");
                    break;
                }

                case MessageOutbound::Type::Exit:
                {
                    shouldExit = true;
                    break;
                }

                default: break;
            }
        }

        if (shouldExit)
            break;

        if (future.wait_for(std::chrono::milliseconds(50)) == std::future_status::ready)
        {
            std::string command = future.get();

            commandHandler.HandleCommand(app, command);
            future = std::async(std::launch::async, StringUtils::GetLineFromCin);
        }
    }

	return 0;
}