#include "SC3StringDecoder.h"
#include "SC3Expression.h"
#include <exception>
#include <iomanip>

// TODO token list instead of fully decoded string vector
// (expression context?)

std::vector<std::string> SC3StringDecoder::decodeStringTableToUtf8() {
  std::vector<std::string> result;

  if (_file.getStringCount() < 1) return result;

  for (SCXTableIndex i = 0; i < _file.getStringCount(); i++) {
    _cursor = (uint8_t*)_file.getPString(i);
    std::stringstream stream;

    if (i == _file.getStringCount() - 1)
      _end = _cursor + _file.getLength();
    else
      _end = _cursor + _file.getStringOffset(i + 1);

    while (_cursor < _end) {
      if (!decodeToken(stream)) break;
    }

    result.push_back(stream.str());
  }

  return result;
}

bool SC3StringDecoder::decodeToken(std::ostream& stream) {
  uintptr_t remaining = _end - _cursor;
  if (remaining == 0) return false;
  // string terminator
  if (remaining == 1 || *_cursor == 0xFF) return false;

  // character
  if (*_cursor >= 0x80) {
    int id = readUint16BE() & 0x7FFF;
    if (id >= _charset.size()) {
      std::stringstream error;
      error << "Character not in charset: " << id;
      throw std::runtime_error(error.str());
    }
    stream << _charset[id];
    return true;
  }

  SC3StringTokenType tokenType = (SC3StringTokenType)*_cursor++;
  remaining = _end - _cursor;

  switch (tokenType) {
    case LineBreak: {
      stream << "[linebreak]";
      break;
    }
    case AltLineBreak: {
      stream << "[alt-linebreak]";
      break;
    }
    case CharacterNameStart: {
      stream << "[name]";
      break;
    }
    case DialogueLineStart: {
      stream << "[line]";
      break;
    }
    case Present: {
      stream << "[%p]";
      break;
    }
    case SetColor: {
      SC3Expression expr(_cursor);
      if (expr.rawLength() > remaining) {
        throw std::runtime_error("Not enough data when parsing string");
      }
      remaining -= expr.rawLength();
      _cursor += expr.rawLength();
      stream << "[color index=\"";
      stream << expr.toString(true);
      stream << "\"]";
      break;
    }
    case Present_ResetAlignment: {
      stream << "[%e]";
      break;
    }
    case RubyBaseStart: {
      stream << "[ruby-base]";
      break;
    }
    case RubyTextStart: {
      stream << "[ruby-text-start]";
      break;
    }
    case RubyTextEnd: {
      stream << "[ruby-text-end]";
      break;
    }
    case SetFontSize: {
      if (remaining < 2) {
        throw std::runtime_error("Not enough data when parsing string");
      }
      remaining -= 2;
      stream << "[font size=\"";
      print(stream, readUint16BE());
      stream << "\"]";
      break;
    }
    case PrintInParallel: {
      stream << "[parallel]";
      break;
    }
    case CenterText: {
      stream << "[center]";
      break;
    }
    case SetTopMargin: {
      if (remaining < 2) {
        throw std::runtime_error("Not enough data when parsing string");
      }
      remaining -= 2;
      stream << "[margin top=\"";
      print(stream, readUint16BE());
      stream << "\"]";
      break;
    }
    case SetLeftMargin: {
      if (remaining < 2) {
        throw std::runtime_error("Not enough data when parsing string");
      }
      remaining -= 2;
      stream << "[margin left=\"";
      print(stream, readUint16BE());
      stream << "\"]";
      break;
    }
    case GetHardcodedValue: {
      if (remaining < 2) {
        throw std::runtime_error("Not enough data when parsing string");
      }
      remaining -= 2;
      stream << "[hardcoded-value index=\"";
      print(stream, readUint16BE());
      stream << "\"]";
      break;
    }
    case EvaluateExpression: {
      SC3Expression expr(_cursor);
      if (expr.rawLength() > remaining) {
        throw std::runtime_error("Not enough data when parsing string");
      }
      remaining -= expr.rawLength();
      _cursor += expr.rawLength();
      stream << "[evaluate expr=\"";
      stream << expr.toString(true);
      stream << "\"]";
      break;
    }
    case AutoForward: {
      stream << "[auto-forward]";
      break;
    }
    case AutoForward_1A: {
      stream << "[auto-forward-1a]";
      break;
    }
    case Present_0x18: {
      stream << "[%18]";
      break;
    }
    case Unk_1E: {
      stream << "[unk-1e]";
      break;
    }
    default: {
      //std::stringstream error;
      //error << "Unrecognized string token " << (int)tokenType;
      //throw std::runtime_error(error.str());
      stream << "[tok-";
      print(stream, tokenType);
      stream << "]";
    }
  }

  return true;
}

uint16_t SC3StringDecoder::readUint16BE() {
  uint16_t result = _cursor[0] << 8 | _cursor[1];
  _cursor = _cursor + 2;
  return result;
}

void SC3StringDecoder::print(std::ostream& stream, uint16_t value) {
  std::ios oldState(nullptr);
  oldState.copyfmt(stream);
  stream << std::hex << std::setw(4) << std::setfill('0') << value;
  stream.copyfmt(oldState);
}
