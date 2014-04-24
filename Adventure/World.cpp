//
// Adventure
//

#include "Buffer.hpp"

#include <Rk/vector.hpp>
#include <Rk/types.hpp>

namespace Ad
{
  struct VacancyChunk
  {
    u16 bits [16][16];
  };

  enum { vchunk_size = sizeof (VacancyChunk) };

  struct MeshChunk
  {
    Buffer buffer;
  };

  class TaskQueue
  {
  public:
    typedef void (*Func) (const void*, void*, u32, u32);

  private:
    struct Task
    {
      Func        func;
      const void* in;
      void*       out;
      u32         params [(sizeof (void*) == 8) ? 2 : 5];
    };

    enum { task_size = sizeof (Task) };

  public:
    void add (Func func, const void* in, void* out, u32 a, u32 b);

  };

  class World
  {
    enum { stage_width = 7, stage_height = 7 };
    VacancyChunk stage_vacancies [stage_width][stage_width][stage_height];
    MeshChunk    stage_meshes    [stage_width][stage_width][stage_height];
    v3i          stage_coords    [stage_width][stage_width][stage_height];

    v3i stage_mins; // stage is hence all of

    static void regen_mesh_task (const VacancyChunk* in, MeshChunk* out, u32, u32)
    {

    }

    void regen_mesh (int x, int y, int z, class TaskQueue& tq)
    {
      tq.add (
        (TaskQueue::Func) regen_mesh_task,
        &stage_vacancies [x][y][z],
        &stage_meshes    [x][y][z],
        0, 0
      );
    }

    void regen_meshes (class TaskQueue& tq)
    {
      auto stage_base = stage_mins % v3i {stage_width,stage_width,stage_height};

      for (int i = 0; i != stage_width; i++)
      {
        for (int j = 0; j != stage_width; j++)
        {
          for (int k = 0; k != stage_height; k++)
          {
            auto offs = v3i {i,j,k} - stage_base;
            auto cpos = stage_mins + offs;

            if (stage_coords [i][j][k] != cpos)
            {
              stage_coords [i][j][k] = cpos;
              regen_mesh (i, j, k, tq);
            }
          }
        }
      }
    }

  };

}
