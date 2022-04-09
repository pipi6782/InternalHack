#include "Global.h"
#include "AimbotMgr.h"
#include "Character.h"

AimbotMgr* AimbotMgr::instance = nullptr;

AimbotMgr::AimbotMgr()
{
    targetIndex = 0;
}

void AimbotMgr::Create()
{
    if (instance == nullptr)
        instance = new AimbotMgr();
}

AimbotMgr* AimbotMgr::Get()
{
    return instance;
}

void AimbotMgr::Destroy()
{
    SafeDelete(instance);
}

void AimbotMgr::AddEntity(Character* entity, int numofEntities)
{
    entities.push_back(entity);
    target = entity;
    targetIndex = (targetIndex + 1) % numofEntities;
}

void AimbotMgr::TraceEntity()
{
    if (target == nullptr || target->health<=0.0f) ChangeTarget();

    Vector3 playerLocation = player->position;
    Vector3 entityLocation = target->position;

    Vector3 rotation = FindRotation(player, target);

    Vector3 temp = player->rotation;
    player->rotation = rotation;   
}

Vector3 AimbotMgr::FindRotation(Character* from, Character* to)
{
    Vector3 direction = to->position - from->position;

    Vector3 result;
    float Yaw = atan2f(direction.y , direction.x) * (180.0f / 3.14159f);
    float Pitch = atan2f((to->position - from->position).z,sqrt(direction.x * direction.x + direction.y * direction.y)) * (180.0f / 3.14159f);
    result.x = Yaw + 90.0f;
    result.y = Pitch;
    result.z = 0.0f;

    return { result.x,result.y,result.z };
}

void AimbotMgr::ChangeTarget()
{
    //제일 가까이 있는 타겟으로 변경
    float length = FLT_MAX;
    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i]->health <= 0.0f) continue;
        if (IsTeamGame() == true && entities[i]->team == player->team) continue;
        Vector3 direction = player->position - entities[i]->position;
        float temp = D3DXVec3Length(&direction);
        if (length > temp)
        {
            targetIndex = i;
            target = entities[i];
            length = temp;
        }
    }
}

bool AimbotMgr::IsTeamGame()
{
    if ((*gameMode == 0 || *gameMode == 4 || *gameMode == 5 || *gameMode == 7 ||
        *gameMode == 11 || *gameMode == 13 || *gameMode == 14 || *gameMode == 16 ||
        *gameMode == 17 || *gameMode == 20 || *gameMode == 21))
        return true;
    else return false;
}
