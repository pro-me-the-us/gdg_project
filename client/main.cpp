#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <cmath>
#include <vector>
#include "Entity.h"
#include "Tile.h"
#include "Tile_Manager.h"
#include "Map.h"
#include "CollisionChecker.h"
#include "networking.h"
#include "Bullet.h"
#include "Bullet_Manager.h"
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/imgui_impl_glfw.h"
#include "../third_party/imgui/imgui_impl_opengl3.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ORIGINAL_TILE_SIZE 16
#define SCALE 3
#define TILE_SIZE ORIGINAL_TILE_SIZE * SCALE
#define MAX_SCREEN_COL 16
#define MAX_SCREEN_ROW 12
#define VELOCITY 5.0f
#define FPS_World 60
#define FPS_Player 6
#define MAX_PLAYERS 6

static void glfw_error_callback(int error, const char* description){
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
}

enum class screenStates{
    mainRoom,
    joinRoom,
    createRoom,
    gameRoom
};

void handleInput(GLFWwindow *gameWindow, Entity *player, int &dirID, bool &isMoving, CollisionChecker CC, Map &map, Tile_Manager &tile_manager)
{
    isMoving = false;
    int futDir = 0;

    if (glfwGetKey(gameWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        dirID = 1;
        futDir = 1;
        isMoving = true;
    }
    else if (glfwGetKey(gameWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        dirID = 2;
        futDir = 2;
        isMoving = true;
    }
    else if (glfwGetKey(gameWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        dirID = 4;
        futDir = 4;
        isMoving = true;
    }
    else if (glfwGetKey(gameWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        dirID = 3;
        futDir = 3;
        isMoving = true;
    }

    if (isMoving)
    {
        player->collisionON = CC.CheckTile(player, futDir, map, tile_manager);
        if (!player->collisionON)
        {
            if (futDir == 1)
                player->attriby += player->vely;
            else if (futDir == 2)
                player->attriby -= player->vely;
            else if (futDir == 3)
                player->attribx -= player->velx;
            else if (futDir == 4)
                player->attribx += player->velx;
        }
    }
    else
        futDir = 0;
}

void draw(Shader &shaderProgram, VAO &VAO1, GLuint texture)
{
    shaderProgram.Activate();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "u_Texture"), 0);
    VAO1.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

GLuint loadTexture(const char *path, const char *name)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &w, &h, &ch, 0);
    std::cout << name << " Texture: " << (data ? "OK" : "FAILED") << std::endl;
    std::cout << "W:" << w << " H:" << h << " CH:" << ch << std::endl;
    if (!data)
    {
        std::cout << "STB Error: " << stbi_failure_reason() << std::endl;
        return -1;
    }
    GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return texture;
}

void Load_Map(const char *path, Map &map)
{
    std::ifstream file(path);
    std::string line;
    if (!file.is_open())
    {
        std::cout << "Failed To Open File\n";
        return;
    }
    int row = 0;
    while (std::getline(file, line) && row < map.maxWorldRow)
    {
        std::stringstream ss(line);
        int tile_ID;
        int col = 0;
        while (ss >> tile_ID && col < map.maxWorldCol)
        {
            map.mapTileNumber[row][col] = tile_ID;
            col++;
        }
        row++;
    }
    file.close();
    std::cout << "Map Loaded Successfully\n";
}

void run(
    GLFWwindow *gameWindow,
    Shader &shaderProgram,
    VAO &VAO1,
    GLuint texture,
    Entity *Player,
    Map &map,
    Tile_Manager &tile_manager,
    bool &isMoving,
    CollisionChecker CC,
    Tile FH,
    Tile NH,
    ENetHost *netHost,
    ENetPeer *serverPeer,
    bool isHost,
    int localPlayerID,
    RemotePlayer *remotePlayers,
    int &playerCount,
    int mapNum,
    Bullet_Manager &bullet_manager,
    int *scores,   // scores[playerID] = kill count, index matches player id
    bool *alive,    // alive[playerID] = is this player still in the game
    char* roomIDBuffer
)
{
    double WorlddrawInterval = 1000000000 / FPS_World;
    double PlayerdrawInterval = 1000000000 / FPS_Player;
    double deltaWorld = 0;
    double deltaPlayer = 0;
    long Worldtimer = 0;
    long Playertimer = 0;
    long drawCount = 0;
    long animationCount = 0;

    auto lastTime = std::chrono::steady_clock::now();

    glm::mat4 projection = glm::ortho(0.0f, 768.0f, 0.0f, 576.0f, 1.0f, -1.0f);
    shaderProgram.Activate();
    int proj_loc = glGetUniformLocation(shaderProgram.ID, "u_Projection");
    int view_loc = glGetUniformLocation(shaderProgram.ID, "u_View");
    int model_loc = glGetUniformLocation(shaderProgram.ID, "u_Model");
    std::cout << "Projection : " << proj_loc << "    Model : " << model_loc << "    View : " << view_loc << std::endl;
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));

    float tileWorldX = MAX_SCREEN_COL * TILE_SIZE;
    float tileWorldY = MAX_SCREEN_ROW * TILE_SIZE;

    glClearColor(0.3f, 0.2f, 0.8f, 1.0f);

    int currID = 1;
    int direction_ID = 1;
    float curr_off_x = 0.0f;
    float curr_off_y = 0.0f;

    int scale_loc = glGetUniformLocation(shaderProgram.ID, "u_UVscale");
    int offset_loc = glGetUniformLocation(shaderProgram.ID, "u_UVoffset");

    GLuint Up = loadTexture("../resources/sprites/spritesheet/up.png", "up");
    GLuint Down = loadTexture("../resources/sprites/spritesheet/down.png", "down");
    GLuint Left = loadTexture("../resources/sprites/spritesheet/left.png", "left");
    GLuint Right = loadTexture("../resources/sprites/spritesheet/right.png", "right");

    GLuint curr_tex = Up;
    glm::vec2 scale;
    glm::vec2 offset;

    float Camx = 0.0f;
    float Camy = 0.0f;

    // timer to control how often we send position over network (20 times per second)
    auto lastNetSend = std::chrono::steady_clock::now();

    bool canShoot = true;
    bool open = true;


    //gettting the host ip so that others can connect
    //this is the local ip that means that ohters must also be on the same wifi
    //therefore this won't work over internet on different wifi
    char hostIP[64] = "No IP";
    if (isHost) {
        std::string ip = getLocalIP();
        strncpy(hostIP, ip.c_str(), sizeof(hostIP));
    }

    while (!glfwWindowShouldClose(gameWindow))
    {

        

        glClear(GL_COLOR_BUFFER_BIT);
        
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = currentTime - lastTime;
        deltaWorld += elapsedTime.count() / WorlddrawInterval;
        deltaPlayer += elapsedTime.count() / PlayerdrawInterval;
        Worldtimer += elapsedTime.count();
        Playertimer += elapsedTime.count();
        lastTime = currentTime;

        float deltaTimeMs = std::chrono::duration<float, std::milli>(elapsedTime).count();

        // poll network events every frame
        ENetEvent event;
        while (enet_host_service(netHost, &event, 0) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                if (isHost)
                {
                    if (playerCount >= MAX_PLAYERS - 1)
                    {
                        enet_peer_disconnect(event.peer, 0);
                        break;
                    }
                    remotePlayers[playerCount].id = playerCount + 1;
                    remotePlayers[playerCount].active = true;
                    event.peer->data = &remotePlayers[playerCount];
                    playerCount++;
                    std::cout << "Player joined. Total: " << playerCount << "\n";

                    // send host position and map number to new client
                    sendPlayerState(event.peer, localPlayerID, Player->attribx, Player->attriby, Player->Health, Player->maxHealth, direction_ID,curr_off_x, curr_off_y);
                    sendMapSync(event.peer, mapNum);
                }
                else
                {
                    std::cout << "Connected to host\n";
                }
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                if (event.packet->dataLength < sizeof(MessageType))
                {
                    enet_packet_destroy(event.packet);
                    break;
                }

                MessageType type = *(MessageType *)event.packet->data;

                switch (type)
                {
                case MSG_PLAYER_STATE:
                {
                    PlayerState *state = (PlayerState *)((char *)event.packet->data + sizeof(MessageType));

                    if (isHost)
                    {
                        RemotePlayer *rp = (RemotePlayer *)event.peer->data;
                        if (rp)
                        {
                            rp->prevX = rp->currentX;
                            rp->prevY = rp->currentY;
                            rp->currentX = state->x;
                            rp->currentY = state->y;
                            rp->t = 0.0f;
                            rp->health = state->health;
                            rp->maxHealth = state->maxHealth;
                            rp->direction = state->direction;
                            rp->offX = state->offX;
                            rp->offY = state->offY;
                        }
                        broadcastPlayerState(netHost, state->id, state->x, state->y, state->health, state->maxHealth, state->direction,curr_off_x, curr_off_y);
                    }
                    else
                    {
                        for (int i = 0; i < playerCount; i++)
                        {
                            if (remotePlayers[i].id == state->id)
                            {

                                //updating the location and animation of each connecting player
                                remotePlayers[i].prevX = remotePlayers[i].currentX;
                                remotePlayers[i].prevY = remotePlayers[i].currentY;
                                remotePlayers[i].currentX = state->x;
                                remotePlayers[i].currentY = state->y;
                                remotePlayers[i].t = 0.0f;
                                remotePlayers[i].health = state->health;
                                remotePlayers[i].maxHealth = state->maxHealth;
                                remotePlayers[i].direction = state->direction;
                                remotePlayers[i].offX = state->offX;
                                remotePlayers[i].offY = state->offY;
                                
                                break;
                            }
                        }
                    }
                    break;
                }

                case MSG_MAP_SYNC:
                {
                    int *num = (int *)((char *)event.packet->data + sizeof(MessageType));
                    std::string new_path = "../resources/Map/map" + std::to_string(*num) + ".txt";
                    Load_Map(new_path.c_str(), map);
                    std::cout << "Map reloaded: " << *num << "\n";
                    break;
                }

                // client fired a bullet, host spawns it and tells everyone
                case MSG_BULLET_FIRE:
                {
                    if (isHost)
                    {
                        BulletFire *bf = (BulletFire *)((char *)event.packet->data + sizeof(MessageType));
                        RemotePlayer *rp = (RemotePlayer *)event.peer->data;
                        int shooterId = rp ? rp->id : -1;

                        // spawn the bullet on host side
                        bullet_manager.Create_Bullet(bf->startx, bf->starty, bf->targetx, bf->targety, shooterId);

                        // tell all clients to also spawn this bullet so everyone sees it
                        BulletSpawn bs;
                        bs.shooterId = shooterId;
                        bs.startx = bf->startx;
                        bs.starty = bf->starty;
                        bs.targetx = bf->targetx;
                        bs.targety = bf->targety;
                        broadcastMsg(netHost, MSG_BULLET_SPAWN, bs);
                    }
                    break;
                }

                // host told us someone fired, spawn it on our side for visuals
                case MSG_BULLET_SPAWN:
                {
                    BulletSpawn *bs = (BulletSpawn *)((char *)event.packet->data + sizeof(MessageType));

                    // don't spawn our own bullet again, we already did that locally
                    if (bs->shooterId != localPlayerID)
                        bullet_manager.Create_Bullet(bs->startx, bs->starty, bs->targetx, bs->targety, bs->shooterId);
                    break;
                }

                // host confirmed a hit, update health on our side
                case MSG_PLAYER_HIT:
                {
                    PlayerHit *ph = (PlayerHit *)((char *)event.packet->data + sizeof(MessageType));

                    if (ph->victimId == localPlayerID)
                        Player->Health = ph->heartsLeft;
                    else
                    {
                        for (int i = 0; i < playerCount; i++)
                        {
                            if (remotePlayers[i].id == ph->victimId)
                            {
                                remotePlayers[i].health = ph->heartsLeft;
                                break;
                            }
                        }
                    }
                    break;
                }

                // host sent updated scores after a kill
                case MSG_SCORE_UPDATE:
                {
                    ScoreUpdate *su = (ScoreUpdate *)((char *)event.packet->data + sizeof(MessageType));
                    memcpy(scores, su->scores, sizeof(int) * MAX_PLAYERS);
                    break;
                }

                default:
                    break;
                }

                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
            {
                if (isHost)
                {
                    RemotePlayer *rp = (RemotePlayer *)event.peer->data;
                    if (rp)
                        rp->active = false;
                    event.peer->data = NULL;
                    std::cout << "Player disconnected\n";
                }
                else
                {
                    std::cout << "Disconnected from host\n";
                }
                break;
            }

            default:
                break;
            }
        }

        // advance interpolation for all active remote players each frame
        for (int i = 0; i < MAX_PLAYERS - 1; i++)
        {
            if (!remotePlayers[i].active)
                continue;
            remotePlayers[i].t += deltaTimeMs / 50.0f;
            if (remotePlayers[i].t > 1.0f)
                remotePlayers[i].t = 1.0f;
        }

        Camx = (Player->attribx) - (TILE_SIZE * MAX_SCREEN_COL) / 2;
        Camy = (Player->attriby) - (TILE_SIZE * MAX_SCREEN_ROW) / 2;

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(-Camx, -Camy, 0.0f));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(tileWorldX, tileWorldY, 0.0f));
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

        scale = glm::vec2(1.0f, 1.0f);
        offset = glm::vec2(0.0f, 0.0f);
        glUniform2f(scale_loc, scale.x, scale.y);
        glUniform2f(offset_loc, offset.x, offset.y);

        // draw all map tiles
        for (int row = 0; row < map.maxWorldRow; row++)
        {
            for (int col = 0; col < map.maxWorldCol; col++)
            {
                int tileID = map.mapTileNumber[row][col];
                if (tileID >= 0 && tileID < (int)tile_manager.tiles.size())
                {
                    GLuint currentTileTexture = tile_manager.tiles[tileID].texture_ID;
                    float tileWorldX = col * TILE_SIZE;
                    float tileWorldY = row * TILE_SIZE;
                    glm::mat4 tile_mat = glm::mat4(1.0f);
                    tile_mat = glm::translate(tile_mat, glm::vec3(tileWorldX + 24.0f, tileWorldY + 24.0f, 0.0f));
                    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(tile_mat));
                    draw(shaderProgram, VAO1, currentTileTexture);
                }
            }
        }

        // draw local player
        if(alive[localPlayerID])
        {
            glm::mat4 player_mat = glm::mat4(1.0f);
            player_mat = glm::translate(player_mat, glm::vec3(Player->attribx, Player->attriby, 0.0f));
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(player_mat));
            scale = glm::vec2(0.5f, 0.5f);
            offset = glm::vec2(0.0f, 0.0f);
            glUniform2f(scale_loc, scale.x, scale.y);
            glUniform2f(offset_loc, curr_off_x, curr_off_y);
            draw(shaderProgram, VAO1, curr_tex);
        }
        // draw all active remote players using interpolated positions
        scale = glm::vec2(0.5f, 0.5f);
        glUniform2f(scale_loc, scale.x, scale.y);
        glUniform2f(offset_loc, 0.0f, 0.0f);
        for (int i = 0; i < MAX_PLAYERS - 1; i++)
        {
            if (!remotePlayers[i].active)
                continue;
            float renderX = netlerp(remotePlayers[i].prevX, remotePlayers[i].currentX, remotePlayers[i].t);
            float renderY = netlerp(remotePlayers[i].prevY, remotePlayers[i].currentY, remotePlayers[i].t);
            glm::mat4 rp_mat = glm::mat4(1.0f);
            rp_mat = glm::translate(rp_mat, glm::vec3(renderX, renderY, 0.0f));
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(rp_mat));

            // pick correct texture based on remote player's direction
            GLuint rp_tex = Up;
            if (remotePlayers[i].direction == 2)
                rp_tex = Down;
            else if (remotePlayers[i].direction == 3)
                rp_tex = Left;
            else if (remotePlayers[i].direction == 4)
                rp_tex = Right;
            glUniform2f(offset_loc, remotePlayers[i].offX, remotePlayers[i].offY);
            draw(shaderProgram, VAO1, rp_tex);

            // draw hearts for this remote player
            scale = glm::vec2(1.0f, 1.0f);
            glUniform2f(scale_loc, scale.x, scale.y);
            glUniform2f(offset_loc, 0.0f, 0.0f);
            float rpTotalWidth = (remotePlayers[i].maxHealth - 1) * 16.0f;
            float rpStartX = renderX - (rpTotalWidth / 2.0f);
            float rpStartY = renderY + 32.0f;
            for (int h = 0; h < remotePlayers[i].maxHealth; h++)
            {
                glm::mat4 heart_mat = glm::mat4(1.0f);
                heart_mat = glm::translate(heart_mat, glm::vec3(rpStartX + (h * 16.0f), rpStartY, 0.0f));
                heart_mat = glm::scale(heart_mat, glm::vec3(0.5f, 0.5f, 1.0f));
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(heart_mat));
                if (h < remotePlayers[i].health)
                    draw(shaderProgram, VAO1, FH.texture_ID);
                else
                    draw(shaderProgram, VAO1, NH.texture_ID);
            }
            // reset scale back for next remote player
            scale = glm::vec2(0.5f, 0.5f);
            glUniform2f(scale_loc, scale.x, scale.y);
        }

        // reset scale and offset back to default
        offset = glm::vec2(0.0f, 0.0f);
        scale = glm::vec2(1.0f, 1.0f);
        glUniform2f(offset_loc, offset.x, offset.y);
        glUniform2f(scale_loc, scale.x, scale.y);

        // draw hearts for local player only if alive
        if(alive[localPlayerID])
        {
            float heartSpace = 16.0f;
            float heightOffset = 32.0f;
            float totalWidth = (Player->maxHealth - 1) * heartSpace;
            float startX = Player->attribx - (totalWidth / 2.0f);
            float startY = Player->attriby + heightOffset;
            for (int i = 0; i < Player->maxHealth; i++)
            {
                glm::mat4 heart_mat = glm::mat4(1.0f);
                heart_mat = glm::translate(heart_mat, glm::vec3(startX + (i * heartSpace), startY, 0.0f));
                heart_mat = glm::scale(heart_mat, glm::vec3(0.5f, 0.5f, 1.0f));
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(heart_mat));
                if (i < Player->Health)
                    draw(shaderProgram, VAO1, FH.texture_ID);
                else
                    draw(shaderProgram, VAO1, NH.texture_ID);
            }
        }

        bullet_manager.Draw_Bullet(shaderProgram, model_loc, VAO1, scale_loc, offset_loc);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(220,30));
        ImGui::SetNextWindowPos(ImVec2(0,0));
        
        ImGui::Begin("JoinGame Window",&open,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
        
        ImGui::SetWindowFontScale(0.8f);

        //printing the host's local ip
        char roomLabel[128];
        snprintf(roomLabel, sizeof(roomLabel), "Room ID : %s", isHost ? hostIP : roomIDBuffer);
        float textWidth = ImGui::CalcTextSize(roomLabel).x;
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - textWidth - 10.0f + 1.0f);
        ImGui::Text("%s", roomLabel);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(gameWindow);
        glfwPollEvents();

        // animation update
        if (deltaPlayer >= 1)
        {
            if (direction_ID == 1)
                curr_tex = Up;
            else if (direction_ID == 2)
                curr_tex = Down;
            else if (direction_ID == 3)
                curr_tex = Left;
            else if (direction_ID == 4)
                curr_tex = Right;

            if (currID == 1 && isMoving)
            {
                curr_off_x = 0.0f;
                curr_off_y = 0.0f;
                currID++;
            }
            else if (currID == 2 && isMoving)
            {
                curr_off_x = 0.5f;
                curr_off_y = 0.0f;
                currID++;
            }
            else if (currID == 3 && isMoving)
            {
                curr_off_x = 0.0f;
                curr_off_y = 0.5f;
                currID++;
            }
            else if (currID == 4 && isMoving)
            {
                curr_off_x = 0.5f;
                curr_off_y = 0.5f;
                currID = 1;
            }
            else
            {
                curr_off_x = 0.0f;
                curr_off_y = 0.0f;
            }

            animationCount++;
            deltaPlayer--;
        }

        if (Playertimer >= 1000000000)
        {
            animationCount = 0;
            Playertimer = 0;
        }

        if (deltaWorld >= 1)
        {
            // player clicked to shoot
            if (glfwGetMouseButton(gameWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && canShoot && alive[localPlayerID])
            {
                canShoot = false;
                double mousex, mousey;
                glfwGetCursorPos(gameWindow, &mousex, &mousey);

                float targetX = (float)mousex + Camx;
                float targetY = (576.0f - (float)mousey) + Camy;

                // spawn bullet locally so we see it immediately without waiting for network
                bullet_manager.Create_Bullet(Player->attribx + 24.0f, Player->attriby + 24.0f, targetX, targetY, localPlayerID);

                if (isHost)
                {
                    // host just tells everyone else to spawn it, no need to send to self
                    BulletSpawn bs;
                    bs.shooterId = localPlayerID;
                    bs.startx = Player->attribx + 24.0f;
                    bs.starty = Player->attriby + 24.0f;
                    bs.targetx = targetX;
                    bs.targety = targetY;
                    broadcastMsg(netHost, MSG_BULLET_SPAWN, bs);
                }
                else
                {
                    // client sends shoot info to host, host will validate and broadcast
                    BulletFire bf;
                    bf.startx = Player->attribx + 24.0f;
                    bf.starty = Player->attriby + 24.0f;
                    bf.targetx = targetX;
                    bf.targety = targetY;
                    sendMsg(serverPeer, MSG_BULLET_FIRE, bf);
                }
            }
            else if (glfwGetMouseButton(gameWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            {
                canShoot = true;
            }

            if(alive[localPlayerID])
                handleInput(gameWindow, Player, direction_ID, isMoving, CC, map, tile_manager);
            else
                isMoving = false;

            bullet_manager.Update_Bullet(map, tile_manager);

            // only host checks hits so there's one source of truth for damage
            if (isHost)
            {
                for (auto &b : bullet_manager.Bullets)
                {
                    if (!b.active) continue;

                    // check if this bullet hit our local player
                    if (b.ownerId != localPlayerID && alive[localPlayerID])
                    {
                        float dx = b.posx - (Player->attribx + 24.0f);
                        float dy = b.posy - (Player->attriby + 24.0f);

                        if (dx * dx + dy * dy < 400.0f) // 20px hit radius
                        {
                            b.active = false;
                            Player->Health--;

                            // tell everyone this player got hit
                            PlayerHit ph;
                            ph.victimId = localPlayerID;
                            ph.shooterId = b.ownerId;
                            ph.heartsLeft = Player->Health;
                            broadcastMsg(netHost, MSG_PLAYER_HIT, ph);

                            // if health is 0 mark them dead and give shooter a point
                            if (Player->Health <= 0)
                            {
                                alive[localPlayerID] = false;
                                scores[b.ownerId]++;

                                ScoreUpdate su;
                                memcpy(su.scores, scores, sizeof(int) * MAX_PLAYERS);
                                broadcastMsg(netHost, MSG_SCORE_UPDATE, su);
                            }
                            continue;
                        }
                    }

                    // check if this bullet hit any remote player
                    for (int i = 0; i < playerCount; i++)
                    {
                        if (!remotePlayers[i].active) continue;
                        if (!alive[remotePlayers[i].id]) continue;
                        if (b.ownerId == remotePlayers[i].id) continue;

                        float dx = b.posx - (remotePlayers[i].currentX + 24.0f);
                        float dy = b.posy - (remotePlayers[i].currentY + 24.0f);

                        if (dx * dx + dy * dy < 400.0f) // 20pxpx hit radius
                        {
                            b.active = false;
                            remotePlayers[i].health--;

                            // tell everyone this remote player got hit
                            PlayerHit ph;
                            ph.victimId = remotePlayers[i].id;
                            ph.shooterId = b.ownerId;
                            ph.heartsLeft = remotePlayers[i].health;
                            broadcastMsg(netHost, MSG_PLAYER_HIT, ph);

                            // if health is 0 mark them dead and give shooter a point
                            if (remotePlayers[i].health <= 0)
                            {
                                alive[remotePlayers[i].id] = false;
                                scores[b.ownerId]++;

                                ScoreUpdate su;
                                memcpy(su.scores, scores, sizeof(int) * MAX_PLAYERS);
                                broadcastMsg(netHost, MSG_SCORE_UPDATE, su);
                            }
                            break;
                        }
                    }
                }
            }

            drawCount++;
            deltaWorld--;
        }

        // send position, health and direction every 50ms (20 times per second)
        auto nowNet = std::chrono::steady_clock::now();
        float netElapsed = std::chrono::duration<float, std::milli>(nowNet - lastNetSend).count();
        if (netElapsed >= 50.0f)
        {
            if (isHost)
                broadcastPlayerState(netHost, localPlayerID, Player->attribx, Player->attriby, Player->Health, Player->maxHealth, direction_ID,curr_off_x, curr_off_y);
            else
                sendPlayerState(serverPeer, localPlayerID, Player->attribx, Player->attriby, Player->Health, Player->maxHealth, direction_ID,curr_off_x, curr_off_y);
            lastNetSend = nowNet;
        }

        if (Worldtimer >= 1000000000)
        {
            
            drawCount = 0;
            Worldtimer = 0;
        }

        if(Player->Health <= 0 && alive[localPlayerID]){
            alive[localPlayerID] = false;
            playerCount--;
            // don't break, keep the loop running so server stays alive
            // just stop rendering/moving the local player
        }
    }
}

void join_menu(GLFWwindow* window,char nameBuffer[],size_t nameBufferSize,char roomIDBuffer[],size_t roomIDBufferSize,int* choice){
    
    bool open = true;

    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImFont* mainfont = io.Fonts -> AddFontFromFileTTF("../resources/Fonts/Pixel.ttf",18.0f);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330 core");


    
    GLuint jsTexture = loadTexture("../resources/JoinSRC/joinSRC.png","Join Screen");
    
    screenStates currentScreen = screenStates::mainRoom;
    

    while(!glfwWindowShouldClose(window) && currentScreen!=screenStates::gameRoom){
        glClearColor(0.07f,0.13f,0.17f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(768,576));
        ImGui::SetNextWindowPos(ImVec2(0,0));
        
        ImGui::Begin("JoinGame Window",&open,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 windowBottomRight = ImVec2(windowPos.x + windowSize.x,windowPos.y+windowSize.y);
        
        ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)jsTexture,windowPos,windowBottomRight,ImVec2(0.0f,1.0f),ImVec2(1.0f,0.0f));
        
        ImGui::PushFont(mainfont);

        switch(currentScreen){
            case screenStates::mainRoom:{
                ImGui::SetCursorPosX(ImGui::GetWindowSize().x/2 - ImGui::CalcTextSize("Enter Your GameTag").x/2);
                ImGui::Text("Enter Your GameTag");
        
                ImGui::InputText("GameTag",nameBuffer,nameBufferSize);
                        
                if(ImGui::Button("Join Room")){
                //button returns true so do the whole save the username-terminate window-change concext thing
                    currentScreen = screenStates::joinRoom;
                }

                ImGui::SameLine(0.0f,60.0f);

                if(ImGui::Button("Create New Room")){
                    currentScreen = screenStates::createRoom;
                }

                break;
            }


            case screenStates::joinRoom:{
                ImGui::Text("Enter Room-ID");
                ImGui::InputText("Room-ID",roomIDBuffer,roomIDBufferSize);
                if(ImGui::Button("Start Game")){
                    currentScreen = screenStates::gameRoom;
                }                
                *choice = 2;
                break;
            }


            case screenStates::createRoom:{
                //generate the roomID as per ENET requirement
                //set the room details such as player numbers, etc
                //launch the gameRoom

                currentScreen = screenStates::gameRoom;
                *choice = 1;
                break;
            }

            case screenStates::gameRoom:{
                //break out of this window and launch the gameroom
                break;
            }
        }

        ImGui::PopFont();
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout<<nameBuffer;
    return;
}

int main()
{
    static char nameBuffer[64] = "";
    static char roomIDBuffer[64]="";
    CollisionChecker CC;
    bool isMoving = false;

    Bullet_Manager bullet_manager = Bullet_Manager();

    Entity *Player = new Entity(VELOCITY, VELOCITY, "Player");
    Player->attribx = 10*TILE_SIZE;
    Player->attriby = 32*TILE_SIZE;
    Player->maxHealth = 3;
    Player->Health = 3;

    if (enet_initialize() != 0)
    {
        std::cout << "ENet failed to initialize\n";
        return -1;
    }
    std::cout << "ENet initialized\n";

    int choice = 0;

    ENetHost *netHost = nullptr;
    ENetPeer *serverPeer = nullptr;
    bool isHost = false;
    int localPlayerID = 0;
    int playerCount = 0;
    RemotePlayer remotePlayers[MAX_PLAYERS - 1];

    for (int i = 0; i < MAX_PLAYERS - 1; i++)
    {
        remotePlayers[i].active = false;
        remotePlayers[i].health = 3;
        remotePlayers[i].maxHealth = 3;
        remotePlayers[i].direction = 1;
    }

    // scores and alive arrays indexed by player id (0 = host, 1-5 = clients)
    int scores[MAX_PLAYERS] = {0};
    bool alive[MAX_PLAYERS];
    for (int i = 0; i < MAX_PLAYERS; i++)
        alive[i] = true;

    // generate map number here so both host and client end up on same map
    srand(time(NULL));
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 3);
    int ran_num = distr(gen);


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float WindowWidth  = TILE_SIZE * MAX_SCREEN_COL;
    float WindowHeight = TILE_SIZE * MAX_SCREEN_ROW;

    GLFWwindow *window = glfwCreateWindow(WindowWidth, WindowHeight, "ArenaShooter", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Window Creation Failed\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, WindowWidth, WindowHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float spriteW = 48.0f / 2.0f;
    float spriteH = 48.0f / 2.0f;

    std::vector<GLfloat> vertices =
    {
        -spriteW,  spriteH, 0.0f, 0.0f, 1.0f,
         spriteW,  spriteH, 0.0f, 1.0f, 1.0f,
         spriteW, -spriteH, 0.0f, 1.0f, 0.0f,
        -spriteW, -spriteH, 0.0f, 0.0f, 0.0f
    };

    std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

    Shader shaderProgram("../resources/shaders/default.vert", "../resources/shaders/default.frag");

    VAO VAO1;
    VAO1.Bind();
    VBO VBO1(vertices, vertices.size() * sizeof(GLfloat));
    EBO EBO1(indices, indices.size() * sizeof(GLuint));
    VAO1.LinkVBO(VBO1, 0, 3, 5 * sizeof(GLfloat), (void *)0);
    VAO1.LinkVBO(VBO1, 1, 2, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    Tile floor = Tile(0);
    Tile wall  = Tile(1);
    Tile water = Tile(2);
    Tile brick = Tile(3);
    Tile fullheart = Tile(4);
    Tile noheart   = Tile(5);
    floor.texture_ID     = loadTexture("../resources/sprites/floor.png", "Floor");
    wall.texture_ID      = loadTexture("../resources/sprites/wall.png", "Wall");
    water.texture_ID     = loadTexture("../resources/sprites/water.png", "Water");
    brick.texture_ID     = loadTexture("../resources/sprites/brick.png", "Brick");
    bullet_manager.texture = loadTexture("../resources/sprites/bullet.png", "Bullet");
    wall.collision       = true;
    water.collision      = true;
    wall.bullet_destroy  = true;
    fullheart.texture_ID = loadTexture("../resources/heart/FullHeart.png", "Full Heart");
    noheart.texture_ID   = loadTexture("../resources/heart/NoHeart.png", "No Heart");

    Tile_Manager tile_manager;
    tile_manager.tiles.push_back(floor);
    tile_manager.tiles.push_back(wall);
    tile_manager.tiles.push_back(brick);
    tile_manager.tiles.push_back(water);

    Map map;
    std::string map_path = "../resources/Map/map" + std::to_string(ran_num) + ".txt";
    Load_Map(map_path.c_str(), map);
    std::cout << "Map" << ran_num << '\n';

    GLuint Player_texture = loadTexture("../resources/sprites/spritesheet/up.png", "up");
    std::cout << "Player Texture ID: " << Player_texture << "\n";

    join_menu(window,nameBuffer,sizeof(nameBuffer),roomIDBuffer,sizeof(roomIDBuffer),&choice);

    if (choice == 1)
    {
        isHost = true;
        localPlayerID = 0;

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = 7777;

        netHost = enet_host_create(&address, MAX_PLAYERS, 2, 0, 0);
        if (!netHost)
        {
            std::cout << "Failed to create server host\n";
            enet_deinitialize();
            return -1;
        }
        std::cout << "Hosting on port 7777. Waiting for players...\n";
    }
    else
    {
        isHost = false;
        localPlayerID = -1;

        netHost = enet_host_create(NULL, 1, 2, 0, 0);
        if (!netHost)
        {
            std::cout << "Failed to create client host\n";
            enet_deinitialize();
            return -1;
        }

        ENetAddress serverAddress;
        serverAddress.port = 7777;
        enet_address_set_host(&serverAddress, roomIDBuffer);

        serverPeer = enet_host_connect(netHost, &serverAddress, 2, 0);
        if (!serverPeer)
        {
            std::cout << "Failed to initiate connection\n";
            enet_host_destroy(netHost);
            enet_deinitialize();
            return -1;
        }
        std::cout << "Connecting to host...\n";

        ENetEvent event;
        if (enet_host_service(netHost, &event, 3000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
        {
            std::cout << "Connected to host successfully\n";
            localPlayerID = 1;

            // pre-add host as remote player since connect event fired before run()
            remotePlayers[0].id = 0;
            remotePlayers[0].active = true;
            remotePlayers[0].currentX = 382;
            remotePlayers[0].currentY = 202;
            remotePlayers[0].health = 3;
            remotePlayers[0].maxHealth = 3;
            remotePlayers[0].direction = 1;
            playerCount = 1;
        }
        else
        {
            std::cout << "Connection to host failed\n";
            enet_peer_reset(serverPeer);
            enet_host_destroy(netHost);
            enet_deinitialize();
            return -1;
        }
    }

    run(
        window, shaderProgram, VAO1, Player_texture,
        Player, map, tile_manager, isMoving, CC, fullheart, noheart,
        netHost, serverPeer, isHost, localPlayerID, remotePlayers, playerCount,
        ran_num, bullet_manager, scores, alive,roomIDBuffer);

    if (serverPeer)
        enet_peer_disconnect_now(serverPeer, 0);
    enet_host_destroy(netHost);
    enet_deinitialize();

    VAO1.Delete();
    VBO1.Delete();
    shaderProgram.Delete();
    glfwTerminate();
    delete (Player);
    return 0;
}