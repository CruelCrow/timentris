#pragma once
#include <exception>

class TimentrisException : public std::exception {};

namespace BrickExceptions {
	class BrickException : public TimentrisException {};
	class WrongMatrixSize : public BrickException {};
	class EmptyVectorOfBricks : public BrickException {};
	class WrongCoordianteAlias : public BrickException {};

}

namespace PieceExceptions {
	class PieceException : public TimentrisException {};
	class WrongArgument : public PieceException {};
	class BrickNotFound : public PieceException {};
	class BrickAlreadyExists : public PieceException {};
	class WrongRotationAngle : public PieceException {};
	class WrongMatrixSize : public PieceException {};
	class WrongFileStructure : public PieceException {}; //some problem with reading Piece from previously saved file
}

namespace CupExceptions {
	class CupException : public TimentrisException {};
	class WrongCupSize : public CupException {};
	class BrickAlreadyExists : public CupException {};
	class BrickNotFound : public CupException {};
	class WrongBrickPosition : public CupException {};
	class PieceDoesNotExists : public CupException {};
	class DuplicateBricks : public CupException {}; //throwed when two SIMILAR bricks are found in the cup; if thrown, we have a serious problem in the game logic
}

namespace TimentrisGameExceptions {
	class TimentrisGameException : public TimentrisException {};
	class CantLoadPieces : public TimentrisGameException {};
	class AlreadyRunning : public TimentrisGameException {};
	class NoRenderAttached : public TimentrisGameException {};
}

namespace ConfigurationExceptions {
	class ConfigurationException : public TimentrisException {};
	class CantOpenConfigurationFile : public ConfigurationException {};
	class CantSaveConfigurationFile : public ConfigurationException {};
	class ConfigurationFileFormatError : public ConfigurationException {};
}

namespace GLWindowExceptions {
	class GLWindowException : public TimentrisException {};
	class WrongScreenParameters : public GLWindowException {};
	class FailedToCreateWindow : public GLWindowException {};
}

namespace OpenGLExceptions {
	class OpenGLException : public TimentrisException {};
	class FailedToRetrieveFunctionAddress : public OpenGLException {};
	class FailedToCreateContext : public OpenGLException {};
	class WrongShaderType : public OpenGLException {};
	class CreatingShaderFailed : public OpenGLException {};
	class CompilingShaderFailed : public OpenGLException {};
	class AttachingShaderFailed : public OpenGLException {};
	class LinkingShaderProgramFailed : public OpenGLException {};
	class ShaderProgramInvalid : public OpenGLException {};
}

namespace RenderExceptions {
	class RenderException : public TimentrisException {};
	class AlreadyRunning : public RenderException {};
	class NoWindowAttached : public RenderException {};
	class NoGameAttached : public RenderException {};
	class NoInputAttached : public RenderException {};
}

namespace InputExceptions {
	class InputException : public TimentrisException {};
	class AlreadyRunning : public InputException {};
	class NoGameAttached : public InputException {};
	class NoRenderAttached : public InputException {};
	class FailedToSetKeyboardHook : public InputException {};
	class FailedToSetMouseHook : public InputException {};
}

