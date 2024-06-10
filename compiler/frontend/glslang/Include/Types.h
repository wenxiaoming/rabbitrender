//
// Copyright (C) 2002-2005  3Dlabs Inc. Ltd.
// Copyright (C) 2012-2016 LunarG, Inc.
// Copyright (C) 2015-2016 Google, Inc.
// Copyright (C) 2017 ARM Limited.
// Modifications Copyright (C) 2020 Advanced Micro Devices, Inc. All rights reserved.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _TYPES_INCLUDED
#define _TYPES_INCLUDED

#include "../Include/Common.h"
#include "../Include/BaseTypes.h"
#include "../Public/ShaderLang.h"
#include "arrays.h"
#include "debug.h"

#include <algorithm>

namespace glslang {

const int GlslangMaxTypeLength = 200;  // TODO: need to print block/struct one member per line, so this can stay bounded

const char* const AnonymousPrefix = "anon@"; // for something like a block whose members can be directly accessed
inline bool IsAnonymous(const TString& name)
{
    return name.compare(0, 5, AnonymousPrefix) == 0;
}

//
// Details within a sampler type
//
enum TSamplerDim {
    EsdNone,
    Esd1D,
    Esd2D,
    Esd3D,
    EsdCube,
    EsdRect,
    EsdBuffer,
    EsdSubpass,  // goes only with non-sampled image (image is true)
    EsdNumDims
};

struct TSampler {   // misnomer now; includes images, textures without sampler, and textures with sampler
    TBasicType type : 8;  // type returned by sampler
    TSamplerDim dim : 8;
    bool    arrayed : 1;
    bool     shadow : 1;
    bool         ms : 1;
    bool      image : 1;  // image, combined should be false
    bool   combined : 1;  // true means texture is combined with a sampler, false means texture with no sampler
    bool    sampler : 1;  // true means a pure sampler, other fields should be clear()

#ifdef GLSLANG_WEB
    bool is1D()          const { return false; }
    bool isBuffer()      const { return false; }
    bool isRect()        const { return false; }
    bool isSubpass()     const { return false; }
    bool isCombined()    const { return true; }
    bool isImage()       const { return false; }
    bool isImageClass()  const { return false; }
    bool isMultiSample() const { return false; }
    bool isExternal()    const { return false; }
    void setExternal(bool e) { }
    bool isYuv()         const { return false; }
#else
    unsigned int vectorSize : 3;  // vector return type size.
    // Some languages support structures as sample results.  Storing the whole structure in the
    // TSampler is too large, so there is an index to a separate table.
    static const unsigned structReturnIndexBits = 4;                        // number of index bits to use.
    static const unsigned structReturnSlots = (1<<structReturnIndexBits)-1; // number of valid values
    static const unsigned noReturnStruct = structReturnSlots;               // value if no return struct type.
    // Index into a language specific table of texture return structures.
    unsigned int structReturnIndex : structReturnIndexBits;

    bool   external : 1;  // GL_OES_EGL_image_external
    bool        yuv : 1;  // GL_EXT_YUV_target

#ifdef ENABLE_HLSL
    unsigned int getVectorSize() const { return vectorSize; }
    void clearReturnStruct() { structReturnIndex = noReturnStruct; }
    bool hasReturnStruct() const { return structReturnIndex != noReturnStruct; }
    unsigned getStructReturnIndex() const { return structReturnIndex; }
#endif

    bool is1D()          const { return dim == Esd1D; }
    bool isBuffer()      const { return dim == EsdBuffer; }
    bool isRect()        const { return dim == EsdRect; }
    bool isSubpass()     const { return dim == EsdSubpass; }
    bool isCombined()    const { return combined; }
    bool isImage()       const { return image && !isSubpass(); }
    bool isImageClass()  const { return image; }
    bool isMultiSample() const { return ms; }
    bool isExternal()    const { return external; }
    void setExternal(bool e) { external = e; }
    bool isYuv()         const { return yuv; }
#endif
    bool isTexture()     const { return !sampler && !image; }
    bool isPureSampler() const { return sampler; }

    void setCombined(bool c) { combined = c; }
    void setBasicType(TBasicType t) { type = t; }
    TBasicType getBasicType()  const { return type; }
    bool isShadow()      const { return shadow; }
    bool isArrayed()     const { return arrayed; }

    void clear()
    {
        type = EbtVoid;
        dim = EsdNone;
        arrayed = false;
        shadow = false;
        ms = false;
        image = false;
        combined = false;
        sampler = false;
#ifndef GLSLANG_WEB
        external = false;
        yuv = false;
#endif

#ifdef ENABLE_HLSL
        clearReturnStruct();
        // by default, returns a single vec4;
        vectorSize = 4;
#endif
    }

    // make a combined sampler and texture
    void set(TBasicType t, TSamplerDim d, bool a = false, bool s = false, bool m = false)
    {
        clear();
        type = t;
        dim = d;
        arrayed = a;
        shadow = s;
        ms = m;
        combined = true;
    }

    // make an image
    void setImage(TBasicType t, TSamplerDim d, bool a = false, bool s = false, bool m = false)
    {
        clear();
        type = t;
        dim = d;
        arrayed = a;
        shadow = s;
        ms = m;
        image = true;
    }

    // make a texture with no sampler
    void setTexture(TBasicType t, TSamplerDim d, bool a = false, bool s = false, bool m = false)
    {
        clear();
        type = t;
        dim = d;
        arrayed = a;
        shadow = s;
        ms = m;
    }

    // make a pure sampler, no texture, no image, nothing combined, the 'sampler' keyword
    void setPureSampler(bool s)
    {
        clear();
        sampler = true;
        shadow = s;
    }

#ifndef GLSLANG_WEB
    // make a subpass input attachment
    void setSubpass(TBasicType t, bool m = false)
    {
        clear();
        type = t;
        image = true;
        dim = EsdSubpass;
        ms = m;
    }
#endif

    bool operator==(const TSampler& right) const
    {
        return      type == right.type &&
                     dim == right.dim &&
                 arrayed == right.arrayed &&
                  shadow == right.shadow &&
         isMultiSample() == right.isMultiSample() &&
          isImageClass() == right.isImageClass() &&
            isCombined() == right.isCombined() &&
         isPureSampler() == right.isPureSampler() &&
            isExternal() == right.isExternal() &&
                 isYuv() == right.isYuv()
#ifdef ENABLE_HLSL
      && getVectorSize() == right.getVectorSize() &&
  getStructReturnIndex() == right.getStructReturnIndex()
#endif
        ;
    }

    bool operator!=(const TSampler& right) const
    {
        return ! operator==(right);
    }

    TString getString() const
    {
        TString s;

        if (isPureSampler()) {
            s.append("sampler");
            return s;
        }

        switch (type) {
        case EbtInt:    s.append("i");   break;
        case EbtUint:   s.append("u");   break;
#ifndef GLSLANG_WEB
        case EbtFloat16: s.append("f16"); break;
        case EbtInt8:   s.append("i8");  break;
        case EbtUint16: s.append("u8");  break;
        case EbtInt16:  s.append("i16"); break;
        case EbtUint8:  s.append("u16"); break;
        case EbtInt64:  s.append("i64"); break;
        case EbtUint64: s.append("u64"); break;
#endif
        default:  break;
        }
        if (isImageClass()) {
            if (isSubpass())
                s.append("subpass");
            else
                s.append("image");
        } else if (isCombined()) {
            s.append("sampler");
        } else {
            s.append("texture");
        }
        if (isExternal()) {
            s.append("ExternalOES");
            return s;
        }
        if (isYuv()) {
            return "__" + s + "External2DY2YEXT";
        }
        switch (dim) {
        case Esd2D:      s.append("2D");      break;
        case Esd3D:      s.append("3D");      break;
        case EsdCube:    s.append("Cube");    break;
#ifndef GLSLANG_WEB
        case Esd1D:      s.append("1D");      break;
        case EsdRect:    s.append("2DRect");  break;
        case EsdBuffer:  s.append("Buffer");  break;
        case EsdSubpass: s.append("Input"); break;
#endif
        default:  break;  // some compilers want this
        }
        if (isMultiSample())
            s.append("MS");
        if (arrayed)
            s.append("Array");
        if (shadow)
            s.append("Shadow");

        return s;
    }
};

//
// Need to have association of line numbers to types in a list for building structs.
//
class TType;
struct TTypeLoc {
    TType* type;
    TSourceLoc loc;
};
typedef TVector<TTypeLoc> TTypeList;

typedef TVector<TString*> TIdentifierList;

//
// Following are a series of helper enums for managing layouts and qualifiers,
// used for TPublicType, TType, others.
//

enum TLayoutPacking {
    ElpNone,
    ElpShared,      // default, but different than saying nothing
    ElpStd140,
    ElpStd430,
    ElpPacked,
    ElpScalar,
    ElpCount        // If expanding, see bitfield width below
};

enum TLayoutMatrix {
    ElmNone,
    ElmRowMajor,
    ElmColumnMajor, // default, but different than saying nothing
    ElmCount        // If expanding, see bitfield width below
};

// Union of geometry shader and tessellation shader geometry types.
// They don't go into TType, but rather have current state per shader or
// active parser type (TPublicType).
enum TLayoutGeometry {
    ElgNone,
    ElgPoints,
    ElgLines,
    ElgLinesAdjacency,
    ElgLineStrip,
    ElgTriangles,
    ElgTrianglesAdjacency,
    ElgTriangleStrip,
    ElgQuads,
    ElgIsolines,
};

enum TVertexSpacing {
    EvsNone,
    EvsEqual,
    EvsFractionalEven,
    EvsFractionalOdd
};

enum TVertexOrder {
    EvoNone,
    EvoCw,
    EvoCcw
};

// Note: order matters, as type of format is done by comparison.
enum TLayoutFormat {
    ElfNone,

    // Float image
    ElfRgba32f,
    ElfRgba16f,
    ElfR32f,
    ElfRgba8,
    ElfRgba8Snorm,

    ElfEsFloatGuard,    // to help with comparisons

    ElfRg32f,
    ElfRg16f,
    ElfR11fG11fB10f,
    ElfR16f,
    ElfRgba16,
    ElfRgb10A2,
    ElfRg16,
    ElfRg8,
    ElfR16,
    ElfR8,
    ElfRgba16Snorm,
    ElfRg16Snorm,
    ElfRg8Snorm,
    ElfR16Snorm,
    ElfR8Snorm,

    ElfFloatGuard,      // to help with comparisons

    // Int image
    ElfRgba32i,
    ElfRgba16i,
    ElfRgba8i,
    ElfR32i,

    ElfEsIntGuard,     // to help with comparisons

    ElfRg32i,
    ElfRg16i,
    ElfRg8i,
    ElfR16i,
    ElfR8i,
    ElfR64i,

    ElfIntGuard,       // to help with comparisons

    // Uint image
    ElfRgba32ui,
    ElfRgba16ui,
    ElfRgba8ui,
    ElfR32ui,

    ElfEsUintGuard,    // to help with comparisons

    ElfRg32ui,
    ElfRg16ui,
    ElfRgb10a2ui,
    ElfRg8ui,
    ElfR16ui,
    ElfR8ui,
    ElfR64ui,

    ElfCount
};

enum TLayoutDepth {
    EldNone,
    EldAny,
    EldGreater,
    EldLess,
    EldUnchanged,

    EldCount
};

enum TBlendEquationShift {
    // No 'EBlendNone':
    // These are used as bit-shift amounts.  A mask of such shifts will have type 'int',
    // and in that space, 0 means no bits set, or none.  In this enum, 0 means (1 << 0), a bit is set.
    EBlendMultiply,
    EBlendScreen,
    EBlendOverlay,
    EBlendDarken,
    EBlendLighten,
    EBlendColordodge,
    EBlendColorburn,
    EBlendHardlight,
    EBlendSoftlight,
    EBlendDifference,
    EBlendExclusion,
    EBlendHslHue,
    EBlendHslSaturation,
    EBlendHslColor,
    EBlendHslLuminosity,
    EBlendAllEquations,

    EBlendCount
};

enum TInterlockOrdering {
    EioNone,
    EioPixelInterlockOrdered,
    EioPixelInterlockUnordered,
    EioSampleInterlockOrdered,
    EioSampleInterlockUnordered,
    EioShadingRateInterlockOrdered,
    EioShadingRateInterlockUnordered,

    EioCount,
};

enum TShaderInterface
{
    // Includes both uniform blocks and buffer blocks
    EsiUniform = 0,
    EsiInput,
    EsiOutput,
    EsiNone,

    EsiCount
};


class TQualifier {
public:
    static const int layoutNotSet = -1;

    void clear()
    {
        precision = EpqNone;
        invariant = false;
        makeTemporary();
        declaredBuiltIn = EbvNone;
#ifndef GLSLANG_WEB
        noContraction = false;
        nullInit = false;
#endif
    }

    // drop qualifiers that don't belong in a temporary variable
    void makeTemporary()
    {
        semanticName = nullptr;
        storage = EvqTemporary;
        builtIn = EbvNone;
        clearInterstage();
        clearMemory();
        specConstant = false;
        nonUniform = false;
        nullInit = false;
        clearLayout();
    }

    void clearInterstage()
    {
        clearInterpolation();
#ifndef GLSLANG_WEB
        patch = false;
        sample = false;
#endif
    }

    void clearInterpolation()
    {
        centroid     = false;
        smooth       = false;
        flat         = false;
#ifndef GLSLANG_WEB
        nopersp      = false;
        explicitInterp = false;
        pervertexNV = false;
        perPrimitiveNV = false;
        perViewNV = false;
        perTaskNV = false;
#endif
    }

    void clearMemory()
    {
#ifndef GLSLANG_WEB
        coherent     = false;
        devicecoherent = false;
        queuefamilycoherent = false;
        workgroupcoherent = false;
        subgroupcoherent  = false;
        shadercallcoherent = false;
        nonprivate = false;
        volatil      = false;
        restrict     = false;
        readonly     = false;
        writeonly    = false;
#endif
    }

    const char*         semanticName;
    TStorageQualifier   storage   : 6;
    TBuiltInVariable    builtIn   : 9;
    TBuiltInVariable    declaredBuiltIn : 9;
    static_assert(EbvLast < 256, "need to increase size of TBuiltInVariable bitfields!");
    TPrecisionQualifier precision : 3;
    bool invariant    : 1; // require canonical treatment for cross-shader invariance
    bool centroid     : 1;
    bool smooth       : 1;
    bool flat         : 1;
    // having a constant_id is not sufficient: expressions have no id, but are still specConstant
    bool specConstant : 1;
    bool nonUniform   : 1;
    bool explicitOffset   : 1;

#ifdef GLSLANG_WEB
    bool isWriteOnly() const { return false; }
    bool isReadOnly() const { return false; }
    bool isRestrict() const { return false; }
    bool isCoherent() const { return false; }
    bool isVolatile() const { return false; }
    bool isSample() const { return false; }
    bool isMemory() const { return false; }
    bool isMemoryQualifierImageAndSSBOOnly() const { return false; }
    bool bufferReferenceNeedsVulkanMemoryModel() const { return false; }
    bool isInterpolation() const { return flat || smooth; }
    bool isExplicitInterpolation() const { return false; }
    bool isAuxiliary() const { return centroid; }
    bool isPatch() const { return false; }
    bool isNoContraction() const { return false; }
    void setNoContraction() { }
    bool isPervertexNV() const { return false; }
    void setNullInit() { }
    bool isNullInit() const { return false; }
#else
    bool noContraction: 1; // prevent contraction and reassociation, e.g., for 'precise' keyword, and expressions it affects
    bool nopersp      : 1;
    bool explicitInterp : 1;
    bool pervertexNV  : 1;
    bool perPrimitiveNV : 1;
    bool perViewNV : 1;
    bool perTaskNV : 1;
    bool patch        : 1;
    bool sample       : 1;
    bool restrict     : 1;
    bool readonly     : 1;
    bool writeonly    : 1;
    bool coherent     : 1;
    bool volatil      : 1;
    bool devicecoherent : 1;
    bool queuefamilycoherent : 1;
    bool workgroupcoherent : 1;
    bool subgroupcoherent  : 1;
    bool shadercallcoherent : 1;
    bool nonprivate   : 1;
    bool nullInit : 1;
    bool isWriteOnly() const { return writeonly; }
    bool isReadOnly() const { return readonly; }
    bool isRestrict() const { return restrict; }
    bool isCoherent() const { return coherent; }
    bool isVolatile() const { return volatil; }
    bool isSample() const { return sample; }
    bool isMemory() const
    {
        return shadercallcoherent || subgroupcoherent || workgroupcoherent || queuefamilycoherent || devicecoherent || coherent || volatil || restrict || readonly || writeonly || nonprivate;
    }
    bool isMemoryQualifierImageAndSSBOOnly() const
    {
        return shadercallcoherent || subgroupcoherent || workgroupcoherent || queuefamilycoherent || devicecoherent || coherent || volatil || restrict || readonly || writeonly;
    }
    bool bufferReferenceNeedsVulkanMemoryModel() const
    {
        // include qualifiers that map to load/store availability/visibility/nonprivate memory access operands
        return subgroupcoherent || workgroupcoherent || queuefamilycoherent || devicecoherent || coherent || nonprivate;
    }
    bool isInterpolation() const
    {
        return flat || smooth || nopersp || explicitInterp;
    }
    bool isExplicitInterpolation() const
    {
        return explicitInterp;
    }
    bool isAuxiliary() const
    {
        return centroid || patch || sample || pervertexNV;
    }
    bool isPatch() const { return patch; }
    bool isNoContraction() const { return noContraction; }
    void setNoContraction() { noContraction = true; }
    bool isPervertexNV() const { return pervertexNV; }
    void setNullInit() { nullInit = true; }
    bool isNullInit() const { return nullInit; }
#endif

    bool isPipeInput() const
    {
        switch (storage) {
        case EvqVaryingIn:
        case EvqFragCoord:
        case EvqPointCoord:
        case EvqFace:
        case EvqVertexId:
        case EvqInstanceId:
            return true;
        default:
            return false;
        }
    }

    bool isPipeOutput() const
    {
        switch (storage) {
        case EvqPosition:
        case EvqPointSize:
        case EvqClipVertex:
        case EvqVaryingOut:
        case EvqFragColor:
        case EvqFragDepth:
            return true;
        default:
            return false;
        }
    }

    bool isParamInput() const
    {
        switch (storage) {
        case EvqIn:
        case EvqInOut:
        case EvqConstReadOnly:
            return true;
        default:
            return false;
        }
    }

    bool isParamOutput() const
    {
        switch (storage) {
        case EvqOut:
        case EvqInOut:
            return true;
        default:
            return false;
        }
    }

    bool isUniformOrBuffer() const
    {
        switch (storage) {
        case EvqUniform:
        case EvqBuffer:
            return true;
        default:
            return false;
        }
    }

    bool isIo() const
    {
        switch (storage) {
        case EvqUniform:
        case EvqBuffer:
        case EvqVaryingIn:
        case EvqFragCoord:
        case EvqPointCoord:
        case EvqFace:
        case EvqVertexId:
        case EvqInstanceId:
        case EvqPosition:
        case EvqPointSize:
        case EvqClipVertex:
        case EvqVaryingOut:
        case EvqFragColor:
        case EvqFragDepth:
            return true;
        default:
            return false;
        }
    }

    // non-built-in symbols that might link between compilation units
    bool isLinkable() const
    {
        switch (storage) {
        case EvqGlobal:
        case EvqVaryingIn:
        case EvqVaryingOut:
        case EvqUniform:
        case EvqBuffer:
        case EvqShared:
            return true;
        default:
            return false;
        }
    }

#ifdef GLSLANG_WEB
    bool isPerView() const { return false; }
    bool isTaskMemory() const { return false; }
    bool isArrayedIo(EShLanguage language) const { return false; }
#else
    bool isPerPrimitive() const { return perPrimitiveNV; }
    bool isPerView() const { return perViewNV; }
    bool isTaskMemory() const { return perTaskNV; }
    bool isAnyPayload() const {
        return storage == EvqPayload || storage == EvqPayloadIn;
    }
    bool isAnyCallable() const {
        return storage == EvqCallableData || storage == EvqCallableDataIn;
    }

    // True if this type of IO is supposed to be arrayed with extra level for per-vertex data
    bool isArrayedIo(EShLanguage language) const
    {
        switch (language) {
        case EShLangGeometry:
            return isPipeInput();
        case EShLangTessControl:
            return ! patch && (isPipeInput() || isPipeOutput());
        case EShLangTessEvaluation:
            return ! patch && isPipeInput();
        case EShLangFragment:
            return pervertexNV && isPipeInput();
        case EShLangMeshNV:
            return ! perTaskNV && isPipeOutput();

        default:
            return false;
        }
    }
#endif

    // Implementing an embedded layout-qualifier class here, since C++ can't have a real class bitfield
    void clearLayout()  // all layout
    {
        clearUniformLayout();

#ifndef GLSLANG_WEB
        layoutPushConstant = false;
        layoutBufferReference = false;
        layoutPassthrough = false;
        layoutViewportRelative = false;
        // -2048 as the default value indicating layoutSecondaryViewportRelative is not set
        layoutSecondaryViewportRelativeOffset = -2048;
        layoutShaderRecord = false;
        layoutBufferReferenceAlign = layoutBufferReferenceAlignEnd;
        layoutFormat = ElfNone;
#endif

        clearInterstageLayout();

        layoutSpecConstantId = layoutSpecConstantIdEnd;
    }
    void clearInterstageLayout()
    {
        layoutLocation = layoutLocationEnd;
        layoutComponent = layoutComponentEnd;
#ifndef GLSLANG_WEB
        layoutIndex = layoutIndexEnd;
        clearStreamLayout();
        clearXfbLayout();
#endif
    }

#ifndef GLSLANG_WEB
    void clearStreamLayout()
    {
        layoutStream = layoutStreamEnd;
    }
    void clearXfbLayout()
    {
        layoutXfbBuffer = layoutXfbBufferEnd;
        layoutXfbStride = layoutXfbStrideEnd;
        layoutXfbOffset = layoutXfbOffsetEnd;
    }
#endif

    bool hasNonXfbLayout() const
    {
        return hasUniformLayout() ||
               hasAnyLocation() ||
               hasStream() ||
               hasFormat() ||
               isShaderRecord() ||
               isPushConstant() ||
               hasBufferReference();
    }
    bool hasLayout() const
    {
        return hasNonXfbLayout() ||
               hasXfb();
    }
    TLayoutMatrix  layoutMatrix  : 3;
    TLayoutPacking layoutPacking : 4;
    int layoutOffset;
    int layoutAlign;

                 unsigned int layoutLocation             : 12;
    static const unsigned int layoutLocationEnd      =  0xFFF;

                 unsigned int layoutComponent            :  3;
    static const unsigned int layoutComponentEnd      =     4;

                 unsigned int layoutSet                  :  7;
    static const unsigned int layoutSetEnd           =   0x3F;

                 unsigned int layoutBinding              : 16;
    static const unsigned int layoutBindingEnd      =  0xFFFF;

                 unsigned int layoutIndex                :  8;
    static const unsigned int layoutIndexEnd      =      0xFF;

                 unsigned int layoutStream               :  8;
    static const unsigned int layoutStreamEnd      =     0xFF;

                 unsigned int layoutXfbBuffer            :  4;
    static const unsigned int layoutXfbBufferEnd      =   0xF;

                 unsigned int layoutXfbStride            : 14;
    static const unsigned int layoutXfbStrideEnd     = 0x3FFF;

                 unsigned int layoutXfbOffset            : 13;
    static const unsigned int layoutXfbOffsetEnd     = 0x1FFF;

                 unsigned int layoutAttachment           :  8;  // for input_attachment_index
    static const unsigned int layoutAttachmentEnd      = 0XFF;

                 unsigned int layoutSpecConstantId       : 11;
    static const unsigned int layoutSpecConstantIdEnd = 0x7FF;

#ifndef GLSLANG_WEB
    // stored as log2 of the actual alignment value
                 unsigned int layoutBufferReferenceAlign :  6;
    static const unsigned int layoutBufferReferenceAlignEnd = 0x3F;

    TLayoutFormat layoutFormat                           :  8;

    bool layoutPushConstant;
    bool layoutBufferReference;
    bool layoutPassthrough;
    bool layoutViewportRelative;
    int layoutSecondaryViewportRelativeOffset;
    bool layoutShaderRecord;
#endif

    bool hasUniformLayout() const
    {
        return hasMatrix() ||
               hasPacking() ||
               hasOffset() ||
               hasBinding() ||
               hasSet() ||
               hasAlign();
    }
    void clearUniformLayout() // only uniform specific
    {
        layoutMatrix = ElmNone;
        layoutPacking = ElpNone;
        layoutOffset = layoutNotSet;
        layoutAlign = layoutNotSet;

        layoutSet = layoutSetEnd;
        layoutBinding = layoutBindingEnd;
#ifndef GLSLANG_WEB
        layoutAttachment = layoutAttachmentEnd;
#endif
    }

    bool hasMatrix() const
    {
        return layoutMatrix != ElmNone;
    }
    bool hasPacking() const
    {
        return layoutPacking != ElpNone;
    }
    bool hasAlign() const
    {
        return layoutAlign != layoutNotSet;
    }
    bool hasAnyLocation() const
    {
        return hasLocation() ||
               hasComponent() ||
               hasIndex();
    }
    bool hasLocation() const
    {
        return layoutLocation != layoutLocationEnd;
    }
    bool hasSet() const
    {
        return layoutSet != layoutSetEnd;
    }
    bool hasBinding() const
    {
        return layoutBinding != layoutBindingEnd;
    }
#ifdef GLSLANG_WEB
    bool hasOffset() const { return false; }
    bool isNonPerspective() const { return false; }
    bool hasIndex() const { return false; }
    unsigned getIndex() const { return 0; }
    bool hasComponent() const { return false; }
    bool hasStream() const { return false; }
    bool hasFormat() const { return false; }
    bool hasXfb() const { return false; }
    bool hasXfbBuffer() const { return false; }
    bool hasXfbStride() const { return false; }
    bool hasXfbOffset() const { return false; }
    bool hasAttachment() const { return false; }
    TLayoutFormat getFormat() const { return ElfNone; }
    bool isPushConstant() const { return false; }
    bool isShaderRecord() const { return false; }
    bool hasBufferReference() const { return false; }
    bool hasBufferReferenceAlign() const { return false; }
    bool isNonUniform() const { return false; }
#else
    bool hasOffset() const
    {
        return layoutOffset != layoutNotSet;
    }
    bool isNonPerspective() const { return nopersp; }
    bool hasIndex() const
    {
        return layoutIndex != layoutIndexEnd;
    }
    unsigned getIndex() const { return layoutIndex; }
    bool hasComponent() const
    {
        return layoutComponent != layoutComponentEnd;
    }
    bool hasStream() const
    {
        return layoutStream != layoutStreamEnd;
    }
    bool hasFormat() const
    {
        return layoutFormat != ElfNone;
    }
    bool hasXfb() const
    {
        return hasXfbBuffer() ||
               hasXfbStride() ||
               hasXfbOffset();
    }
    bool hasXfbBuffer() const
    {
        return layoutXfbBuffer != layoutXfbBufferEnd;
    }
    bool hasXfbStride() const
    {
        return layoutXfbStride != layoutXfbStrideEnd;
    }
    bool hasXfbOffset() const
    {
        return layoutXfbOffset != layoutXfbOffsetEnd;
    }
    bool hasAttachment() const
    {
        return layoutAttachment != layoutAttachmentEnd;
    }
    TLayoutFormat getFormat() const { return layoutFormat; }
    bool isPushConstant() const { return layoutPushConstant; }
    bool isShaderRecord() const { return layoutShaderRecord; }
    bool hasBufferReference() const { return layoutBufferReference; }
    bool hasBufferReferenceAlign() const
    {
        return layoutBufferReferenceAlign != layoutBufferReferenceAlignEnd;
    }
    bool isNonUniform() const
    {
        return nonUniform;
    }
#endif
    bool hasSpecConstantId() const
    {
        // Not the same thing as being a specialization constant, this
        // is just whether or not it was declared with an ID.
        return layoutSpecConstantId != layoutSpecConstantIdEnd;
    }
    bool isSpecConstant() const
    {
        // True if type is a specialization constant, whether or not it
        // had a specialization-constant ID, and false if it is not a
        // true front-end constant.
        return specConstant;
    }
    bool isFrontEndConstant() const
    {
        // True if the front-end knows the final constant value.
        // This allows front-end constant folding.
        return storage == EvqConst && ! specConstant;
    }
    bool isConstant() const
    {
        // True if is either kind of constant; specialization or regular.
        return isFrontEndConstant() || isSpecConstant();
    }
    void makeSpecConstant()
    {
        storage = EvqConst;
        specConstant = true;
    }
    static const char* getLayoutPackingString(TLayoutPacking packing)
    {
        switch (packing) {
        case ElpStd140:   return "std140";
#ifndef GLSLANG_WEB
        case ElpPacked:   return "packed";
        case ElpShared:   return "shared";
        case ElpStd430:   return "std430";
        case ElpScalar:   return "scalar";
#endif
        default:          return "none";
        }
    }
    static const char* getLayoutMatrixString(TLayoutMatrix m)
    {
        switch (m) {
        case ElmColumnMajor: return "column_major";
        case ElmRowMajor:    return "row_major";
        default:             return "none";
        }
    }
#ifdef GLSLANG_WEB
    static const char* getLayoutFormatString(TLayoutFormat f) { return "none"; }
#else
    static const char* getLayoutFormatString(TLayoutFormat f)
    {
        switch (f) {
        case ElfRgba32f:      return "rgba32f";
        case ElfRgba16f:      return "rgba16f";
        case ElfRg32f:        return "rg32f";
        case ElfRg16f:        return "rg16f";
        case ElfR11fG11fB10f: return "r11f_g11f_b10f";
        case ElfR32f:         return "r32f";
        case ElfR16f:         return "r16f";
        case ElfRgba16:       return "rgba16";
        case ElfRgb10A2:      return "rgb10_a2";
        case ElfRgba8:        return "rgba8";
        case ElfRg16:         return "rg16";
        case ElfRg8:          return "rg8";
        case ElfR16:          return "r16";
        case ElfR8:           return "r8";
        case ElfRgba16Snorm:  return "rgba16_snorm";
        case ElfRgba8Snorm:   return "rgba8_snorm";
        case ElfRg16Snorm:    return "rg16_snorm";
        case ElfRg8Snorm:     return "rg8_snorm";
        case ElfR16Snorm:     return "r16_snorm";
        case ElfR8Snorm:      return "r8_snorm";

        case ElfRgba32i:      return "rgba32i";
        case ElfRgba16i:      return "rgba16i";
        case ElfRgba8i:       return "rgba8i";
        case ElfRg32i:        return "rg32i";
        case ElfRg16i:        return "rg16i";
        case ElfRg8i:         return "rg8i";
        case ElfR32i:         return "r32i";
        case ElfR16i:         return "r16i";
        case ElfR8i:          return "r8i";

        case ElfRgba32ui:     return "rgba32ui";
        case ElfRgba16ui:     return "rgba16ui";
        case ElfRgba8ui:      return "rgba8ui";
        case ElfRg32ui:       return "rg32ui";
        case ElfRg16ui:       return "rg16ui";
        case ElfRgb10a2ui:    return "rgb10_a2ui";
        case ElfRg8ui:        return "rg8ui";
        case ElfR32ui:        return "r32ui";
        case ElfR16ui:        return "r16ui";
        case ElfR8ui:         return "r8ui";
        case ElfR64ui:        return "r64ui";
        case ElfR64i:         return "r64i";
        default:              return "none";
        }
    }
    static const char* getLayoutDepthString(TLayoutDepth d)
    {
        switch (d) {
        case EldAny:       return "depth_any";
        case EldGreater:   return "depth_greater";
        case EldLess:      return "depth_less";
        case EldUnchanged: return "depth_unchanged";
        default:           return "none";
        }
    }
    static const char* getBlendEquationString(TBlendEquationShift e)
    {
        switch (e) {
        case EBlendMultiply:      return "blend_support_multiply";
        case EBlendScreen:        return "blend_support_screen";
        case EBlendOverlay:       return "blend_support_overlay";
        case EBlendDarken:        return "blend_support_darken";
        case EBlendLighten:       return "blend_support_lighten";
        case EBlendColordodge:    return "blend_support_colordodge";
        case EBlendColorburn:     return "blend_support_colorburn";
        case EBlendHardlight:     return "blend_support_hardlight";
        case EBlendSoftlight:     return "blend_support_softlight";
        case EBlendDifference:    return "blend_support_difference";
        case EBlendExclusion:     return "blend_support_exclusion";
        case EBlendHslHue:        return "blend_support_hsl_hue";
        case EBlendHslSaturation: return "blend_support_hsl_saturation";
        case EBlendHslColor:      return "blend_support_hsl_color";
        case EBlendHslLuminosity: return "blend_support_hsl_luminosity";
        case EBlendAllEquations:  return "blend_support_all_equations";
        default:                  return "unknown";
        }
    }
    static const char* getGeometryString(TLayoutGeometry geometry)
    {
        switch (geometry) {
        case ElgPoints:             return "points";
        case ElgLines:              return "lines";
        case ElgLinesAdjacency:     return "lines_adjacency";
        case ElgLineStrip:          return "line_strip";
        case ElgTriangles:          return "triangles";
        case ElgTrianglesAdjacency: return "triangles_adjacency";
        case ElgTriangleStrip:      return "triangle_strip";
        case ElgQuads:              return "quads";
        case ElgIsolines:           return "isolines";
        default:                    return "none";
        }
    }
    static const char* getVertexSpacingString(TVertexSpacing spacing)
    {
        switch (spacing) {
        case EvsEqual:              return "equal_spacing";
        case EvsFractionalEven:     return "fractional_even_spacing";
        case EvsFractionalOdd:      return "fractional_odd_spacing";
        default:                    return "none";
        }
    }
    static const char* getVertexOrderString(TVertexOrder order)
    {
        switch (order) {
        case EvoCw:                 return "cw";
        case EvoCcw:                return "ccw";
        default:                    return "none";
        }
    }
    static int mapGeometryToSize(TLayoutGeometry geometry)
    {
        switch (geometry) {
        case ElgPoints:             return 1;
        case ElgLines:              return 2;
        case ElgLinesAdjacency:     return 4;
        case ElgTriangles:          return 3;
        case ElgTrianglesAdjacency: return 6;
        default:                    return 0;
        }
    }
    static const char* getInterlockOrderingString(TInterlockOrdering order)
    {
        switch (order) {
        case EioPixelInterlockOrdered:          return "pixel_interlock_ordered";
        case EioPixelInterlockUnordered:        return "pixel_interlock_unordered";
        case EioSampleInterlockOrdered:         return "sample_interlock_ordered";
        case EioSampleInterlockUnordered:       return "sample_interlock_unordered";
        case EioShadingRateInterlockOrdered:    return "shading_rate_interlock_ordered";
        case EioShadingRateInterlockUnordered:  return "shading_rate_interlock_unordered";
        default:                                return "none";
        }
    }
#endif
};

// Qualifiers that don't need to be keep per object.  They have shader scope, not object scope.
// So, they will not be part of TType, TQualifier, etc.
struct TShaderQualifiers {
    TLayoutGeometry geometry; // geometry/tessellation shader in/out primitives
    bool pixelCenterInteger;  // fragment shader
    bool originUpperLeft;     // fragment shader
    int invocations;
    int vertices;             // for tessellation "vertices", geometry & mesh "max_vertices"
    TVertexSpacing spacing;
    TVertexOrder order;
    bool pointMode;
    int localSize[3];         // compute shader
    bool localSizeNotDefault[3];        // compute shader
    int localSizeSpecId[3];   // compute shader specialization id for gl_WorkGroupSize
#ifndef GLSLANG_WEB
    bool earlyFragmentTests;  // fragment input
    bool postDepthCoverage;   // fragment input
    TLayoutDepth layoutDepth;
    bool blendEquation;       // true if any blend equation was specified
    int numViews;             // multiview extenstions
    TInterlockOrdering interlockOrdering;
    bool layoutOverrideCoverage;        // true if layout override_coverage set
    bool layoutDerivativeGroupQuads;    // true if layout derivative_group_quadsNV set
    bool layoutDerivativeGroupLinear;   // true if layout derivative_group_linearNV set
    int primitives;                     // mesh shader "max_primitives"DerivativeGroupLinear;   // true if layout derivative_group_linearNV set
    bool layoutPrimitiveCulling;        // true if layout primitive_culling set
    TLayoutDepth getDepth() const { return layoutDepth; }
#else
    TLayoutDepth getDepth() const { return EldNone; }
#endif

    void init()
    {
        geometry = ElgNone;
        originUpperLeft = false;
        pixelCenterInteger = false;
        invocations = TQualifier::layoutNotSet;
        vertices = TQualifier::layoutNotSet;
        spacing = EvsNone;
        order = EvoNone;
        pointMode = false;
        localSize[0] = 1;
        localSize[1] = 1;
        localSize[2] = 1;
        localSizeNotDefault[0] = false;
        localSizeNotDefault[1] = false;
        localSizeNotDefault[2] = false;
        localSizeSpecId[0] = TQualifier::layoutNotSet;
        localSizeSpecId[1] = TQualifier::layoutNotSet;
        localSizeSpecId[2] = TQualifier::layoutNotSet;
#ifndef GLSLANG_WEB
        earlyFragmentTests = false;
        postDepthCoverage = false;
        layoutDepth = EldNone;
        blendEquation = false;
        numViews = TQualifier::layoutNotSet;
        layoutOverrideCoverage      = false;
        layoutDerivativeGroupQuads  = false;
        layoutDerivativeGroupLinear = false;
        layoutPrimitiveCulling      = false;
        primitives                  = TQualifier::layoutNotSet;
        interlockOrdering = EioNone;
#endif
    }

#ifdef GLSLANG_WEB
    bool hasBlendEquation() const { return false; }
#else
    bool hasBlendEquation() const { return blendEquation; }
#endif

    // Merge in characteristics from the 'src' qualifier.  They can override when
    // set, but never erase when not set.
    void merge(const TShaderQualifiers& src)
    {
        if (src.geometry != ElgNone)
            geometry = src.geometry;
        if (src.pixelCenterInteger)
            pixelCenterInteger = src.pixelCenterInteger;
        if (src.originUpperLeft)
            originUpperLeft = src.originUpperLeft;
        if (src.invocations != TQualifier::layoutNotSet)
            invocations = src.invocations;
        if (src.vertices != TQualifier::layoutNotSet)
            vertices = src.vertices;
        if (src.spacing != EvsNone)
            spacing = src.spacing;
        if (src.order != EvoNone)
            order = src.order;
        if (src.pointMode)
            pointMode = true;
        for (int i = 0; i < 3; ++i) {
            if (src.localSize[i] > 1)
                localSize[i] = src.localSize[i];
        }
        for (int i = 0; i < 3; ++i) {
            localSizeNotDefault[i] = src.localSizeNotDefault[i] || localSizeNotDefault[i];
        }
        for (int i = 0; i < 3; ++i) {
            if (src.localSizeSpecId[i] != TQualifier::layoutNotSet)
                localSizeSpecId[i] = src.localSizeSpecId[i];
        }
#ifndef GLSLANG_WEB
        if (src.earlyFragmentTests)
            earlyFragmentTests = true;
        if (src.postDepthCoverage)
            postDepthCoverage = true;
        if (src.layoutDepth)
            layoutDepth = src.layoutDepth;
        if (src.blendEquation)
            blendEquation = src.blendEquation;
        if (src.numViews != TQualifier::layoutNotSet)
            numViews = src.numViews;
        if (src.layoutOverrideCoverage)
            layoutOverrideCoverage = src.layoutOverrideCoverage;
        if (src.layoutDerivativeGroupQuads)
            layoutDerivativeGroupQuads = src.layoutDerivativeGroupQuads;
        if (src.layoutDerivativeGroupLinear)
            layoutDerivativeGroupLinear = src.layoutDerivativeGroupLinear;
        if (src.primitives != TQualifier::layoutNotSet)
            primitives = src.primitives;
        if (src.interlockOrdering != EioNone)
            interlockOrdering = src.interlockOrdering;
        if (src.layoutPrimitiveCulling)
            layoutPrimitiveCulling = src.layoutPrimitiveCulling;
#endif
    }
};

//
// TPublicType is just temporarily used while parsing and not quite the same
// information kept per node in TType.  Due to the bison stack, it can't have
// types that it thinks have non-trivial constructors.  It should
// just be used while recognizing the grammar, not anything else.
// Once enough is known about the situation, the proper information
// moved into a TType, or the parse context, etc.
//
class TPublicType {
public:
    TBasicType basicType;
    TSampler sampler;
    TQualifier qualifier;
    TShaderQualifiers shaderQualifiers;
    int vectorSize : 4;
    int matrixCols : 4;
    int matrixRows : 4;
    bool coopmat   : 1;
    TArraySizes* arraySizes;
    const TType* userDef;
    TSourceLoc loc;
    TArraySizes* typeParameters;

#ifdef GLSLANG_WEB
    bool isCoopmat() const { return false; }
#else
    bool isCoopmat() const { return coopmat; }
#endif

    void initType(const TSourceLoc& l)
    {
        basicType = EbtVoid;
        vectorSize = 1;
        matrixRows = 0;
        matrixCols = 0;
        arraySizes = nullptr;
        userDef = nullptr;
        loc = l;
        typeParameters = nullptr;
        coopmat = false;
    }

    void initQualifiers(bool global = false)
    {
        qualifier.clear();
        if (global)
            qualifier.storage = EvqGlobal;
    }

    void init(const TSourceLoc& l, bool global = false)
    {
        initType(l);
        sampler.clear();
        initQualifiers(global);
        shaderQualifiers.init();
    }

    void setVector(int s)
    {
        matrixRows = 0;
        matrixCols = 0;
        vectorSize = s;
    }

    void setMatrix(int c, int r)
    {
        matrixRows = r;
        matrixCols = c;
        vectorSize = 0;
    }

    bool isScalar() const
    {
        return matrixCols == 0 && vectorSize == 1 && arraySizes == nullptr && userDef == nullptr;
    }

    // "Image" is a superset of "Subpass"
    bool isImage()   const { return basicType == EbtSampler && sampler.isImage(); }
    bool isSubpass() const { return basicType == EbtSampler && sampler.isSubpass(); }
};

class TField {
  public:
    POOL_ALLOCATOR_NEW_DELETE(GetThreadPoolAllocator())
    TField(TType *type, TString *name, const TSourceLoc &line)
        : mType(type), mName(name), mLine(line) {}

    // TODO(alokp): We should only return const type.
    // Fix it by tweaking grammar.
    TType *type() { return mType; }
    const TType *type() const { return mType; }

    const TString &name() const { return *mName; }
    const TSourceLoc &line() const { return mLine; }

  private:
    TType *mType;
    TString *mName;
    TSourceLoc mLine;
};

typedef TVector<TField *> TFieldList;
inline TFieldList *NewPoolTFieldList() {
    void *memory = GetGlobalPoolAllocator()->allocate(sizeof(TFieldList));
    return new (memory) TFieldList;
}

class TFieldListCollection {
  public:
    virtual ~TFieldListCollection() {}
    const TString &name() const { return *mName; }
    const TFieldList &fields() const { return *mFields; }

    const TString &mangledName() const {
        if (mMangledName.empty())
            mMangledName = buildMangledName();
        return mMangledName;
    }
    size_t objectSize() const {
        if (mObjectSize == 0)
            mObjectSize = calculateObjectSize();
        return mObjectSize;
    }

  protected:
    TFieldListCollection(const TString *name, TFieldList *fields)
        : mName(name), mFields(fields), mObjectSize(0) {}
    TString buildMangledName() const;
    size_t calculateObjectSize() const;
    virtual TString mangledNamePrefix() const = 0;

    const TString *mName;
    TFieldList *mFields;

    mutable TString mMangledName;
    mutable size_t mObjectSize;
};

// May also represent interface blocks
class TStructure : public TFieldListCollection {
  public:
    POOL_ALLOCATOR_NEW_DELETE(GetThreadPoolAllocator())
    TStructure(const TString *name, TFieldList *fields)
        : TFieldListCollection(name, fields), mDeepestNesting(0), mUniqueId(0),
          mAtGlobalScope(false) {}

    int deepestNesting() const {
        if (mDeepestNesting == 0)
            mDeepestNesting = calculateDeepestNesting();
        return mDeepestNesting;
    }
    bool containsArrays() const;
    bool containsType(TBasicType type) const;
    bool containsSamplers() const;

    bool equals(const TStructure &other) const;

    void setMatrixPackingIfUnspecified(TLayoutMatrixPacking matrixPacking);

    void setUniqueId(int uniqueId) { mUniqueId = uniqueId; }

    int uniqueId() const {
        ASSERT(mUniqueId != 0);
        return mUniqueId;
    }

    void setAtGlobalScope(bool atGlobalScope) {
        mAtGlobalScope = atGlobalScope;
    }

    bool atGlobalScope() const { return mAtGlobalScope; }

  private:
    // TODO(zmo): Find a way to get rid of the const_cast in function
    // setName().  At the moment keep this function private so only
    // friend class RegenerateStructNames may call it.
    friend class RegenerateStructNames;
    void setName(const TString &name) {
        TString *mutableName = const_cast<TString *>(mName);
        *mutableName = name;
    }

    virtual TString mangledNamePrefix() const { return "struct-"; }
    int calculateDeepestNesting() const;

    mutable int mDeepestNesting;
    int mUniqueId;
    bool mAtGlobalScope;
};

class TInterfaceBlock : public TFieldListCollection {
  public:
    POOL_ALLOCATOR_NEW_DELETE(GetThreadPoolAllocator())
    TInterfaceBlock(const TString *name, TFieldList *fields,
                    const TString *instanceName, int arraySize,
                    const TLayoutQualifier &layoutQualifier)
        : TFieldListCollection(name, fields), mInstanceName(instanceName),
          mArraySize(arraySize), mBlockStorage(layoutQualifier.blockStorage),
          mMatrixPacking(layoutQualifier.matrixPacking) {}

    const TString &instanceName() const { return *mInstanceName; }
    bool hasInstanceName() const { return mInstanceName != nullptr; }
    bool isArray() const { return mArraySize > 0; }
    int arraySize() const { return mArraySize; }
    TLayoutBlockStorage blockStorage() const { return mBlockStorage; }
    TLayoutMatrixPacking matrixPacking() const { return mMatrixPacking; }

  private:
    virtual TString mangledNamePrefix() const { return "iblock-"; }

    const TString *mInstanceName; // for interface block instance names
    int mArraySize;               // 0 if not an array
    TLayoutBlockStorage mBlockStorage;
    TLayoutMatrixPacking mMatrixPacking;
};

//
// Base class for things that have a type.
//
class TType
{
public:
    POOL_ALLOCATOR_NEW_DELETE(GetThreadPoolAllocator())

	TType(TBasicType t, int s0 = 1, int s1 = 1) :
		type(t), precision(EbpUndefined), qualifier(EvqGlobal),
		primarySize(s0), secondarySize(s1), array(false), arraySize(0), maxArraySize(0), arrayInformationType(0), interfaceBlock(0), layoutQualifier(TLayoutQualifier::create()),
		structure(0), mangled(0)
	{
	}

	TType(TBasicType t, TPrecision p, TStorageQualifier q = EvqTemporary, int s0 = 1, int s1 = 1, bool a = false) :
		type(t), precision(p), qualifier(q),
		primarySize(s0), secondarySize(s1), array(a), arraySize(0), maxArraySize(0), arrayInformationType(0), interfaceBlock(0), layoutQualifier(TLayoutQualifier::create()),
		structure(0), mangled(0)
	{
	}

	TType(TStructure* userDef, TPrecision p = EbpUndefined) :
		type(EbtStruct), precision(p), qualifier(EvqTemporary),
		primarySize(1), secondarySize(1), array(false), arraySize(0), maxArraySize(0), arrayInformationType(0), interfaceBlock(0), layoutQualifier(TLayoutQualifier::create()),
		structure(userDef), mangled(0)
	{
	}

	TType(TInterfaceBlock *interfaceBlockIn, TStorageQualifier qualifierIn,
		TLayoutQualifier layoutQualifierIn, int arraySizeIn)
		: type(EbtInterfaceBlock), precision(EbpUndefined), qualifier(qualifierIn),
		primarySize(1), secondarySize(1), array(arraySizeIn > 0), arraySize(arraySizeIn), maxArraySize(0), arrayInformationType(0),
		interfaceBlock(interfaceBlockIn), layoutQualifier(layoutQualifierIn), structure(0), mangled(0)
	{
	}

	explicit TType(const TPublicType &p);

	TBasicType getBasicType() const { return type; }
	void setBasicType(TBasicType t) { type = t; }

	TPrecision getPrecision() const { return precision; }
	void setPrecision(TPrecision p) { precision = p; }

	TStorageQualifier getQualifier() const { return qualifier; }
    void setQualifier(TStorageQualifier q) { qualifier = q; }

	TLayoutQualifier getLayoutQualifier() const { return layoutQualifier; }
	void setLayoutQualifier(TLayoutQualifier lq) { layoutQualifier = lq; }

	void setMatrixPackingIfUnspecified(TLayoutMatrixPacking matrixPacking)
	{
		if(isStruct())
		{
			// If the structure's matrix packing is specified, it overrules the block's matrix packing
			structure->setMatrixPackingIfUnspecified((layoutQualifier.matrixPacking == EmpUnspecified) ?
			                                         matrixPacking : layoutQualifier.matrixPacking);
		}
		// If the member's matrix packing is specified, it overrules any higher level matrix packing
		if(layoutQualifier.matrixPacking == EmpUnspecified)
		{
			layoutQualifier.matrixPacking = matrixPacking;
		}
	}

	// One-dimensional size of single instance type
	int getNominalSize() const { return primarySize; }
	void setNominalSize(int s) { primarySize = s; }
	// Full size of single instance of type
	size_t getObjectSize() const
	{
		if(isArray())
		{
			return getElementSize() * std::max(getArraySize(), getMaxArraySize());
		}
		else
		{
			return getElementSize();
		}
	}

	size_t getElementSize() const
	{
		if(getBasicType() == EbtStruct)
		{
			return getStructSize();
		}
		else if(isInterfaceBlock())
		{
			return interfaceBlock->objectSize();
		}
		else if(isMatrix())
		{
			return primarySize * secondarySize;
		}
		else   // Vector or scalar
		{
			return primarySize;
		}
	}

	int samplerRegisterCount() const
	{
		if(structure)
		{
			int registerCount = 0;

			const TFieldList& fields = isInterfaceBlock() ? interfaceBlock->fields() : structure->fields();
			for(size_t i = 0; i < fields.size(); i++)
			{
				registerCount += fields[i]->type()->totalSamplerRegisterCount();
			}

			return registerCount;
		}

		return IsSampler(getBasicType()) ? 1 : 0;
	}

	int elementRegisterCount() const
	{
		if(structure || isInterfaceBlock())
		{
			int registerCount = 0;

			const TFieldList& fields = isInterfaceBlock() ? interfaceBlock->fields() : structure->fields();
			for(size_t i = 0; i < fields.size(); i++)
			{
				registerCount += fields[i]->type()->totalRegisterCount();
			}

			return registerCount;
		}
		else if(isMatrix())
		{
			return getNominalSize();
		}
		else
		{
			return 1;
		}
	}

	int blockRegisterCount() const
	{
		// If this TType object is a block member, return the register count of the parent block
		// Otherwise, return the register count of the current TType object
		if(interfaceBlock && !isInterfaceBlock())
		{
			int registerCount = 0;
			const TFieldList& fieldList = interfaceBlock->fields();
			for(size_t i = 0; i < fieldList.size(); i++)
			{
				const TType &fieldType = *(fieldList[i]->type());
				registerCount += fieldType.totalRegisterCount();
			}
			return registerCount;
		}
		return totalRegisterCount();
	}

	int totalSamplerRegisterCount() const
	{
		if(array)
		{
			return arraySize * samplerRegisterCount();
		}
		else
		{
			return samplerRegisterCount();
		}
	}

	int totalRegisterCount() const
	{
		if(array)
		{
			return arraySize * elementRegisterCount();
		}
		else
		{
			return elementRegisterCount();
		}
	}

	int registerSize() const
	{
		return isMatrix() ? secondarySize : primarySize;
	}

	bool isMatrix() const { return secondarySize > 1; }
	void setSecondarySize(int s1) { secondarySize = s1; }
	int getSecondarySize() const { return secondarySize; }

	bool isArray() const  { return array ? true : false; }
	bool isUnsizedArray() const { return array && arraySize == 0; }
	int getArraySize() const { return arraySize; }
	void setArraySize(int s) { array = true; arraySize = s; }
	int getMaxArraySize () const { return maxArraySize; }
	void setMaxArraySize (int s) { maxArraySize = s; }
	void clearArrayness() { array = false; arraySize = 0; maxArraySize = 0; }
	void setArrayInformationType(TType* t) { arrayInformationType = t; }
	TType* getArrayInformationType() const { return arrayInformationType; }

	TInterfaceBlock *getInterfaceBlock() const { return interfaceBlock; }
	void setInterfaceBlock(TInterfaceBlock *interfaceBlockIn) { interfaceBlock = interfaceBlockIn; }
	bool isInterfaceBlock() const { return type == EbtInterfaceBlock; }
	TInterfaceBlock *getAsInterfaceBlock() const { return isInterfaceBlock() ? getInterfaceBlock() : nullptr; }

	bool isVector() const { return primarySize > 1 && !isMatrix(); }
	bool isScalar() const { return primarySize == 1 && !isMatrix() && !structure && !isInterfaceBlock() && !IsSampler(getBasicType()); }
	bool isRegister() const { return !isMatrix() && !structure && !array && !isInterfaceBlock(); }   // Fits in a 4-element register
	bool isStruct() const { return structure != 0; }
	bool isScalarInt() const { return isScalar() && IsInteger(type); }

	TStructure* getStruct() const { return structure; }
	void setStruct(TStructure* s) { structure = s; }

	TString& getMangledName() {
		if (!mangled) {
			mangled = NewPoolTString("");
			buildMangledName(*mangled);
			*mangled += ';' ;
		}

		return *mangled;
	}

	bool sameElementType(const TType& right) const {
		return      type == right.type   &&
		     primarySize == right.primarySize &&
		   secondarySize == right.secondarySize &&
		       structure == right.structure;
	}
	bool operator==(const TType& right) const {
		return      type == right.type   &&
		     primarySize == right.primarySize &&
		   secondarySize == right.secondarySize &&
			       array == right.array && (!array || arraySize == right.arraySize) &&
		       structure == right.structure;
		// don't check the qualifier, it's not ever what's being sought after
	}
	bool operator!=(const TType& right) const {
		return !operator==(right);
	}
	bool operator<(const TType& right) const {
		if (type != right.type) return type < right.type;
		if(primarySize != right.primarySize) return (primarySize * secondarySize) < (right.primarySize * right.secondarySize);
		if(secondarySize != right.secondarySize) return secondarySize < right.secondarySize;
		if (array != right.array) return array < right.array;
		if (arraySize != right.arraySize) return arraySize < right.arraySize;
		if (structure != right.structure) return structure < right.structure;

		return false;
	}

	const char* getBasicString() const { return ::getBasicString(type); }
	const char* getPrecisionString() const { return ::getPrecisionString(precision); }
	const char* getQualifierString() const { return ::getQualifierString(qualifier); }
	TString getCompleteString() const;

	// If this type is a struct, returns the deepest struct nesting of
	// any field in the struct. For example:
	//   struct nesting1 {
	//     vec4 position;
	//   };
	//   struct nesting2 {
	//     nesting1 field1;
	//     vec4 field2;
	//   };
	// For type "nesting2", this method would return 2 -- the number
	// of structures through which indirection must occur to reach the
	// deepest field (nesting2.field1.position).
	int getDeepestStructNesting() const
	{
		return structure ? structure->deepestNesting() : 0;
	}

	bool isStructureContainingArrays() const
	{
		return structure ? structure->containsArrays() : false;
	}

	bool isStructureContainingType(TBasicType t) const
	{
		return structure ? structure->containsType(t) : false;
	}

	bool isStructureContainingSamplers() const
	{
		return structure ? structure->containsSamplers() : false;
	}

protected:
	void buildMangledName(TString&);
	size_t getStructSize() const;

	TBasicType type = EbtVoid;
	TPrecision precision = EbpUndefined;
     TStorageQualifier qualifier = EvqTemporary;
	unsigned char primarySize = 0;     // size of vector or matrix, not size of array
	unsigned char secondarySize = 0;   // 1 for vectors, > 1 for matrices
	bool array = false;
	int arraySize = 0;
	int maxArraySize = 0;
	TType *arrayInformationType = nullptr;

	// null unless this is an interface block, or interface block member variable
	TInterfaceBlock *interfaceBlock = nullptr;
	TLayoutQualifier layoutQualifier;

	TStructure *structure = nullptr;   // null unless this is a struct

	TString *mangled = nullptr;
};

} // end namespace glslang

#endif // _TYPES_INCLUDED_
