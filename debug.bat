cmake -B Debug ^
-DCMAKE_BUILD_TYPE=Debug ^
-DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
-DCMAKE_PREFIX_PATH=C:/QtDebug/lib/cmake ^
-DZENO_MULTIPROCESS=ON ^
-DZENO_SYSTEM_OPENVDB=OFF ^
-DZENO_SYSTEM_ALEMBIC=OFF ^
-DZENO_WITH_ZenoFX:BOOL=ON ^
-DZENOFX_ENABLE_OPENVDB:BOOL=ON ^
-DZENOFX_ENABLE_LBVH:BOOL=ON ^
-DZENO_WITH_zenvdb:BOOL=ON ^
-DZENO_WITH_FastFLIP:BOOL=ON ^
-DZENO_WITH_FEM:BOOL=ON ^
-DZENO_WITH_TOOL_FLIPtools:BOOL=ON ^
-DZENO_WITH_TOOL_cgmeshTools:BOOL=ON ^
-DZENO_WITH_TOOL_BulletTools:BOOL=ON ^
-DZENO_WITH_TOOL_HerculesTools:BOOL=ON ^
-DZENO_WITH_Rigid:BOOL=ON ^
-DZENO_WITH_cgmesh:BOOL=ON ^
-DZENO_WITH_oldzenbase:BOOL=ON ^
-DZENO_WITH_TreeSketch:BOOL=ON ^
-DZENO_WITH_Skinning:BOOL=ON ^
-DZENO_WITH_Euler:BOOL=ON ^
-DZENO_WITH_Functional:BOOL=ON ^
-DZENO_WITH_LSystem:BOOL=ON ^
-DZENO_WITH_mesher:BOOL=ON ^
-DZENO_WITH_Alembic:BOOL=ON ^
-DZENO_WITH_FBX:BOOL=ON ^
-DZENO_WITH_DemBones:BOOL=ON ^
-DZENO_WITH_CalcGeometryUV:BOOL=ON ^
-DZENO_WITH_SampleModel:BOOL=ON ^
-DZENO_WITH_MeshSubdiv:BOOL=ON ^
-DZENO_WITH_CUDA:BOOL=OFF ^
-DZENO_ENABLE_OPTIX:BOOL=ON ^
-DZENO_WITH_Audio:BOOL=ON