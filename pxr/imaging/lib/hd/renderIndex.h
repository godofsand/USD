//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef HD_RENDER_INDEX_H
#define HD_RENDER_INDEX_H

#include "pxr/imaging/hd/api.h"
#include "pxr/imaging/hd/version.h"
#include "pxr/imaging/hd/changeTracker.h"
#include "pxr/imaging/hd/perfLog.h"
#include "pxr/base/gf/vec4i.h"

#include "pxr/usd/sdf/path.h"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr.hpp>
#include "pxr/base/tf/hashmap.h"

#include <vector>

class HdDrawItem;
class HdRprimCollection;
class HdSceneDelegate;

typedef boost::shared_ptr<class HdLight> HdLightSharedPtr;
typedef boost::shared_ptr<class HdDrawTarget> HdDrawTargetSharedPtr;
typedef boost::shared_ptr<class HdCamera> HdCameraSharedPtr;
typedef boost::shared_ptr<class HdDirtyList> HdDirtyListSharedPtr;
typedef boost::shared_ptr<class HdRprim> HdRprimSharedPtr;
typedef boost::shared_ptr<class HdRprim const> HdRprimConstSharedPtr;
typedef boost::shared_ptr<class HdInstancer> HdInstancerSharedPtr;
typedef boost::shared_ptr<class HdSurfaceShader> HdSurfaceShaderSharedPtr;
typedef boost::shared_ptr<class HdTask> HdTaskSharedPtr;
typedef boost::shared_ptr<class HdTexture> HdTextureSharedPtr;

/// The mapping from client scenegraph to the render engine's scene.
///
/// The HdRenderIndex only tracks primitives that result in draw calls and
/// relies on the HdSceneDelegate to provide any hierarchical or other
/// computed values.
///
class HdRenderIndex : public boost::noncopyable {
public:

    // XXX: These should return iterator ranges, not vectors;
    //      they also shouldn't be pointers.
    typedef std::vector<HdDrawItem const*> HdDrawItemView;
    typedef std::vector<HdLightSharedPtr> HdLightView;
    typedef std::vector<HdDrawTargetSharedPtr> HdDrawTargetView;

	HDLIB_API
    HdRenderIndex();
	HDLIB_API
    ~HdRenderIndex();

    /// Clear all rprims, instancers, shaders and textures.
	HDLIB_API
    void Clear();

    /// Given a prim id and instance id, returns the prim path of the owner
	HDLIB_API
    SdfPath GetPrimPathFromPrimIdColor(GfVec4i const& primIdColor,
                                       GfVec4i const& instanceIdColor,
                                       int* instanceIndexOut = NULL) const;
 
    // ---------------------------------------------------------------------- //
    /// \name Synchronization
    // ---------------------------------------------------------------------- //
	HDLIB_API
    HdDrawItemView GetDrawItems(HdRprimCollection const& collection);

	HDLIB_API
    HdLightView GetLights();

    /// Returns a list of all draw targets in the render index.
	HDLIB_API
    HdDrawTargetView GetDrawTargets();

    /// Synchronize all objects in the DirtyList
	HDLIB_API
    void Sync(HdDirtyListSharedPtr const &dirtyList);

    /// Processes all pending dirty lists 
	HDLIB_API
    void SyncAll();

    /// Synchronize all cameras in the render index
	HDLIB_API
    void SyncCameras();

    /// Synchronize all lights in the render index
	HDLIB_API
    void SyncLights();

    /// Synchronize all draw targets in the render index
	HDLIB_API
    void SyncDrawTargets();

    /// Returns a vector of Rprim IDs that are bound to the given DelegateID.
	HDLIB_API
    SdfPathVector const& GetDelegateRprimIDs(SdfPath const& delegateID) const;

    /// For each delegate that has at least one child with dirty bits matching
    /// the given dirtyMask, pushes the delegate ID into the given IDs vector.
    /// The resulting vector is a list of all delegate IDs who have at least one
    /// child that matches the mask.
	HDLIB_API
    void GetDelegateIDsWithDirtyRprims(int dirtyMask, SdfPathVector* IDs) const;

    // ---------------------------------------------------------------------- //
    /// \name Change Tracker
    // ---------------------------------------------------------------------- //

    HdChangeTracker& GetChangeTracker() { return _tracker; }
    HdChangeTracker const& GetChangeTracker() const { return _tracker; }

    // ---------------------------------------------------------------------- //
    /// \name Rprim Support
    // ---------------------------------------------------------------------- //

    /// Insert a rprim into index
    template <typename T>
    void InsertRprim(HdSceneDelegate* delegate,
                     SdfPath const& id,
                     SdfPath const& surfaceShaderId,
                     SdfPath const& instancerId = SdfPath());

    /// Remove a rprim from index
	HDLIB_API
    void RemoveRprim(SdfPath const& id);

    /// Returns true if rprim \p id exists in index.
    bool HasRprim(SdfPath const& id) { 
        return _rprimMap.find(id) != _rprimMap.end();
    }

    /// Returns the rprim of id
	HDLIB_API
    HdRprimSharedPtr const &GetRprim(SdfPath const &id) const;

    /// Returns true if the given RprimID is a member of the collection.
	HDLIB_API
    bool IsInCollection(SdfPath const& id, TfToken const& collectionName) const;

    /// Returns the subtree rooted under the given path.
	HDLIB_API
    SdfPathVector GetRprimSubtree(SdfPath const& root) const;

    // ---------------------------------------------------------------------- //
    /// \name Instancer Support
    // ---------------------------------------------------------------------- //

    /// Insert an instancer into index
	HDLIB_API
    void InsertInstancer(HdSceneDelegate* delegate,
                         SdfPath const &id,
                         SdfPath const &parentId = SdfPath());

    /// Remove an instancer from index
	HDLIB_API
    void RemoveInstancer(SdfPath const& id);

    /// Returns true if instancer \p id exists in index.
    bool HasInstancer(SdfPath const& id) {
        return _instancerMap.find(id) != _instancerMap.end();
    }

    /// Returns the instancer of id
	HDLIB_API
    HdInstancerSharedPtr GetInstancer(SdfPath const &id) const;

    // ---------------------------------------------------------------------- //
    /// \name Shader Support
    // ---------------------------------------------------------------------- //

    /// Inserts a new shader into the render index with an identifer of \p id.
    /// Note that Rprims can be speculatively bound to a shader before the
    /// shader has been inserted into the render index, however the shader must
    /// exist before any Rprims to which it is bound are rendered.
    template <typename T>
    void InsertShader(HdSceneDelegate* delegate, SdfPath const& id);

    /// Removes the given shader from the RenderIndex. The client must unbind
    /// or remove any existing Rprims that are bound to this shader before
    /// rendering.
	HDLIB_API
    void RemoveShader(SdfPath const& id);

    /// Returns true if a shader exists in the index with the given \p id.
    bool HasShader(SdfPath const& id) {
        return _shaderMap.find(id) != _shaderMap.end();
    }

    /// Returns the shader for the given \p id.
	HDLIB_API
    HdSurfaceShaderSharedPtr const& GetShader(SdfPath const& id) const;

    /// Returns the fallback shader.
    HdSurfaceShaderSharedPtr const& GetShaderFallback() const {
        return _surfaceFallback;
    };

    // ---------------------------------------------------------------------- //
    /// \name Task Support
    // ---------------------------------------------------------------------- //

    /// Inserts a new task into the render index with an identifer of \p id.
    template <typename T>
    void InsertTask(HdSceneDelegate* delegate, SdfPath const& id);

    /// Removes the given task from the RenderIndex.
	HDLIB_API
    void RemoveTask(SdfPath const& id);

    /// Returns true if a task exists in the index with the given \p id.
    bool HasTask(SdfPath const& id) {
        return _taskMap.find(id) != _taskMap.end();
    }

    /// Returns the task for the given \p id.
	HDLIB_API
    HdTaskSharedPtr const& GetTask(SdfPath const& id) const;

    // ---------------------------------------------------------------------- //
    /// \name Texture Support
    // ---------------------------------------------------------------------- //

    /// Inserts a new texture into the RenderIndex with an identifier of \p id.
    template <typename T>
    void InsertTexture(HdSceneDelegate* delegate, SdfPath const& id);

    /// Removes the given texture from the RenderIndex.
	HDLIB_API
    void RemoveTexture(SdfPath const& id);

    /// Returns true if a texture exists in the index with the given \p id.
    bool HasTexture(SdfPath const& id) {
        return _textureMap.find(id) != _textureMap.end();
    }

    /// Returns the texture for the given \p id.
	HDLIB_API
    HdTextureSharedPtr const& GetTexture(SdfPath const& id) const;

    // ---------------------------------------------------------------------- //
    /// \name Camera Support
    // ---------------------------------------------------------------------- //

    /// Inserts a new camera into the RenderIndex with an identifier of \p id.
    template <typename T>
    void 
    InsertCamera(HdSceneDelegate* delegate, SdfPath const& id);

    /// Removes the given camera from the RenderIndex.
	HDLIB_API
    void RemoveCamera(SdfPath const& id);

    /// Returns the camera for the given \p id.
	HDLIB_API
    HdCameraSharedPtr const& GetCamera(SdfPath const& id) const;

    // ---------------------------------------------------------------------- //
    /// \name Light Support
    // ---------------------------------------------------------------------- //

    /// Inserts a new light into the RenderIndex with an identifier of \p id.
    template <typename T>
    void
    InsertLight(HdSceneDelegate* delegate, SdfPath const& id);

    /// Removes the given light from the RenderIndex.
	HDLIB_API
    void RemoveLight(SdfPath const& id);

    /// Returns the light for the given \p id.
	HDLIB_API
    HdLightSharedPtr const& GetLight(SdfPath const& id) const;

    // ---------------------------------------------------------------------- //
    /// \name Draw Target Support
    // ---------------------------------------------------------------------- //

    /// Inserts a new draw target into the RenderIndex with an identifier of
    /// \p id.
    template <typename T>
    void
    InsertDrawTarget(HdSceneDelegate* delegate, SdfPath const& id);

    /// Removes the given draw target from the RenderIndex.
	HDLIB_API
    void RemoveDrawTarget(SdfPath const& id);

	HDLIB_API
    HdDrawTargetSharedPtr const& GetDrawTarget(SdfPath const& id) const;

private:
    // ---------------------------------------------------------------------- //
    // Private Helper methods 
    // ---------------------------------------------------------------------- //

    // Go through all RPrims and reallocate their instance ids
    // This is called once we have exhausted all all 24bit instance ids.
    void _CompactPrimIds();

    // Allocate the next available instance id to the prim
    void _AllocatePrimId(HdRprimSharedPtr prim);
    
    // Insert rprimID into the delegateRprimMap.
	HDLIB_API
    void _TrackDelegateRprim(HdSceneDelegate* delegate, 
                             SdfPath const& rprimID,
                             HdRprimSharedPtr const& rprim);

    // Inserts the shader into the index and updates tracking state.
	HDLIB_API
    void _TrackDelegateShader(HdSceneDelegate* delegate, 
                              SdfPath const& shaderId,
                              HdSurfaceShaderSharedPtr const& shader);

    // Inserts the task into the index and updates tracking state.
	HDLIB_API
    void _TrackDelegateTask(HdSceneDelegate* delegate, 
                            SdfPath const& taskId,
                            HdTaskSharedPtr const& task);

    // Inserts the texture into the index and updates tracking state.
	HDLIB_API
    void _TrackDelegateTexture(HdSceneDelegate* delegate, 
                              SdfPath const& textureId,
                              HdTextureSharedPtr const& texture);

    // Inserts the camera into the index and updates tracking state.
	HDLIB_API
    void _TrackDelegateCamera(HdSceneDelegate* delegate, 
                              SdfPath const& cameraId,
                              HdCameraSharedPtr const& camera);

    // Inserts the light into the index and updates tracking state.
	HDLIB_API
    void _TrackDelegateLight(HdSceneDelegate* delegate, 
                              SdfPath const& lightId,
                              HdLightSharedPtr const& light);

    // Inserts the draw target into the index and updates tracking state.
	HDLIB_API
    void _TrackDelegateDrawTarget(HdSceneDelegate* delegate,
                                  SdfPath const& drawTargetId,
                                  HdDrawTargetSharedPtr const& drawTarget);


    // ---------------------------------------------------------------------- //
    // Index State
    // ---------------------------------------------------------------------- //
    struct _RprimInfo {
        SdfPath delegateID;
        size_t childIndex;
        HdRprimSharedPtr rprim;
    };
    
    typedef TfHashMap<SdfPath, HdSurfaceShaderSharedPtr, SdfPath::Hash> _ShaderMap;
    typedef TfHashMap<SdfPath, HdTaskSharedPtr, SdfPath::Hash> _TaskMap;
    typedef TfHashMap<SdfPath, HdTextureSharedPtr, SdfPath::Hash> _TextureMap;
    typedef TfHashMap<SdfPath, _RprimInfo, SdfPath::Hash> _RprimMap;
    typedef TfHashMap<SdfPath, SdfPathVector, SdfPath::Hash> _DelegateRprimMap;
    typedef TfHashMap<SdfPath, HdCameraSharedPtr, SdfPath::Hash> _CameraMap;
    typedef TfHashMap<SdfPath, HdLightSharedPtr, SdfPath::Hash> _LightMap;
    typedef TfHashMap<SdfPath, HdDrawTargetSharedPtr, SdfPath::Hash> _DrawTargetMap;

    typedef std::set<SdfPath> _RprimIDSet;
    typedef std::map<uint32_t, SdfPath> _RprimPrimIDMap;

    _DelegateRprimMap _delegateRprimMap;
    _RprimMap _rprimMap;

    _RprimIDSet _rprimIDSet;
    _RprimPrimIDMap _rprimPrimIdMap;

    _ShaderMap _shaderMap;
    _TaskMap _taskMap;
    _TextureMap _textureMap;
    _CameraMap _cameraMap;
    _LightMap _lightMap;
    _DrawTargetMap _drawTargetMap;

    HdChangeTracker _tracker;
    int32_t _nextPrimId; 

    typedef TfHashMap<SdfPath, HdInstancerSharedPtr, SdfPath::Hash> _InstancerMap;
    _InstancerMap _instancerMap;
    HdSurfaceShaderSharedPtr _surfaceFallback;

    typedef std::vector<HdDirtyListSharedPtr> _DirtyListVector;
    _DirtyListVector _syncQueue;
};

template <typename T>
void
HdRenderIndex::InsertRprim(HdSceneDelegate* delegate, SdfPath const& id,
                           SdfPath const& surfaceShaderId, 
                           SdfPath const& instancerId)
{
    HD_TRACE_FUNCTION();
    HD_MALLOC_TAG_FUNCTION();
#if 0
    // TODO: enable this after patching.
    if (not id.IsAbsolutePath()) {
        TF_CODING_ERROR("All Rprim IDs must be absolute paths <%s>\n",
                id.GetText());
        return;
    }
#endif

    if (ARCH_UNLIKELY(TfMapLookupPtr(_rprimMap, id)))
        return;
    
    HdRprimSharedPtr rprim = boost::make_shared<T>(delegate, id, 
                                                  surfaceShaderId, instancerId);
    _TrackDelegateRprim(delegate, id, rprim);
}

template <typename T>
void
HdRenderIndex::InsertShader(HdSceneDelegate* delegate, SdfPath const& id)
{
    HD_TRACE_FUNCTION();
    HD_MALLOC_TAG_FUNCTION();

    HdSurfaceShaderSharedPtr shader = boost::make_shared<T>(delegate, id);
    _TrackDelegateShader(delegate, id, shader);
}

template <typename T>
void
HdRenderIndex::InsertTask(HdSceneDelegate* delegate, SdfPath const& id)
{
    HD_TRACE_FUNCTION();
    HD_MALLOC_TAG_FUNCTION();

    HdTaskSharedPtr task = boost::make_shared<T>(delegate, id);
    _TrackDelegateTask(delegate, id, task);
}

template <typename T>
void
HdRenderIndex::InsertTexture(HdSceneDelegate* delegate, SdfPath const& id)
{
    HD_TRACE_FUNCTION();
    HD_MALLOC_TAG_FUNCTION();

    HdTextureSharedPtr texture = boost::make_shared<T>(delegate, id);
    _TrackDelegateTexture(delegate, id, texture);
}

template <typename T>
void
HdRenderIndex::InsertCamera(HdSceneDelegate* delegate, SdfPath const& id)
{
    HD_TRACE_FUNCTION();
    HD_MALLOC_TAG_FUNCTION();

    boost::shared_ptr<T> camera = boost::make_shared<T>(delegate, id);
    _TrackDelegateCamera(delegate, id, camera);
}

template <typename T>
void
HdRenderIndex::InsertLight(HdSceneDelegate* delegate, SdfPath const& id)
{
    HD_TRACE_FUNCTION();
    HD_MALLOC_TAG_FUNCTION();

    boost::shared_ptr<T> light = boost::make_shared<T>(delegate, id);
    _TrackDelegateLight(delegate, id, light);
}

template <typename T>
void
HdRenderIndex::InsertDrawTarget(HdSceneDelegate* delegate, SdfPath const& id)
{
    HD_TRACE_FUNCTION();
    HD_MALLOC_TAG_FUNCTION();

    boost::shared_ptr<T> drawTarget = boost::make_shared<T>(delegate, id);
    _TrackDelegateDrawTarget(delegate, id, drawTarget);
}
#endif //HD_RENDER_INDEX_H