#pragma once

struct OpenGLState {
	GLint boundTextures[32];
	GLint currentProgram;
	GLint arrayBuffer;
	GLint elementArrayBuffer;
	GLint blendSrc;
	GLint blendDst;
	GLint blendSrcAlpha;
	GLint blendDstAlpha;
	GLint blendEquationRGB;
	GLint blendEquationAlpha;
	GLboolean blendEnabled;
	GLint frameBuffer;
	GLint viewport[4];
	GLint scissorBox[4];
	GLboolean scissorTestEnabled;
	GLboolean depthTestEnabled;
	GLboolean cullFaceEnabled;
	GLint depthFunc;
	GLint polygonMode[2];
	GLint activeTexture;
};
class OpenGLStateHelper {
public:
	static void printOpenGLState(const char* tag = "GL State") {
		printf("\n==== %s ====\n", tag);

		GLfloat modelview[16], projection[16];

		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		glGetFloatv(GL_PROJECTION_MATRIX, projection);

		printf("ModelView Matrix:\n");
		for (int i = 0; i < 4; ++i)
			printf("  %.3f %.3f %.3f %.3f\n", modelview[i], modelview[i + 4], modelview[i + 8], modelview[i + 12]);

		printf("Projection Matrix:\n");
		for (int i = 0; i < 4; ++i)
			printf("  %.3f %.3f %.3f %.3f\n", projection[i], projection[i + 4], projection[i + 8], projection[i + 12]);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		printf("Viewport: x=%d y=%d w=%d h=%d\n", viewport[0], viewport[1], viewport[2], viewport[3]);

		GLint tex;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex);
		printf("Bound Texture ID: %d\n", tex);

		GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
		GLboolean blend = glIsEnabled(GL_BLEND);
		GLboolean cullFace = glIsEnabled(GL_CULL_FACE);
		GLboolean texture2D = glIsEnabled(GL_TEXTURE_2D);

		printf("GL States:\n");
		printf("  Depth Test: %s\n", depthTest ? "ENABLED" : "disabled");
		printf("  Blend:      %s\n", blend ? "ENABLED" : "disabled");
		printf("  Cull Face:  %s\n", cullFace ? "ENABLED" : "disabled");
		printf("  Texture2D:  %s\n", texture2D ? "ENABLED" : "disabled");

		GLfloat clearColor[4];
		glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
		printf("Clear Color: RGBA(%.2f, %.2f, %.2f, %.2f)\n",
			clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

		GLint sfactor, dfactor;
		glGetIntegerv(GL_BLEND_SRC, &sfactor);
		glGetIntegerv(GL_BLEND_DST, &dfactor);
		printf("Blend Func: src=0x%X, dst=0x%X\n", sfactor, dfactor);

		printf("======================\n");
	}

	static void saveState()
	{
		for (int i = 0; i < 32; ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &state.boundTextures[i]);
		}
		glActiveTexture(GL_TEXTURE0);
		glGetIntegerv(GL_CURRENT_PROGRAM, &state.currentProgram);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &state.arrayBuffer);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &state.elementArrayBuffer);

		glGetIntegerv(GL_BLEND_SRC_RGB, &state.blendSrc);
		glGetIntegerv(GL_BLEND_DST_RGB, &state.blendDst);
		glGetIntegerv(GL_BLEND_SRC_ALPHA, &state.blendSrcAlpha);
		glGetIntegerv(GL_BLEND_DST_ALPHA, &state.blendDstAlpha);
		glGetIntegerv(GL_BLEND_EQUATION_RGB, &state.blendEquationRGB);
		glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &state.blendEquationAlpha);
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &state.frameBuffer);

		glGetIntegerv(GL_VIEWPORT, state.viewport);
		glGetIntegerv(GL_SCISSOR_BOX, state.scissorBox);
		glGetBooleanv(GL_SCISSOR_TEST, &state.scissorTestEnabled);
		glGetBooleanv(GL_CULL_FACE, &state.cullFaceEnabled);

		state.blendEnabled = glIsEnabled(GL_BLEND);
		glGetIntegerv(GL_POLYGON_MODE, state.polygonMode);
		glGetIntegerv(GL_ACTIVE_TEXTURE, &state.activeTexture);
	}

	static void pushState()
	{
		for (int i = 0; i < 32; ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, state.boundTextures[i]);
		}
		glActiveTexture(state.activeTexture);
		glUseProgram(state.currentProgram);
		glBindBuffer(GL_ARRAY_BUFFER, state.arrayBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.elementArrayBuffer);

		if (state.blendEnabled) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}

		glBlendFuncSeparate(state.blendSrc, state.blendDst, state.blendSrcAlpha, state.blendDstAlpha);
		glBlendEquationSeparate(state.blendEquationRGB, state.blendEquationAlpha);
		glBindFramebuffer(GL_FRAMEBUFFER, state.frameBuffer);

		glViewport(state.viewport[0], state.viewport[1], state.viewport[2], state.viewport[3]);
		glScissor(state.scissorBox[0], state.scissorBox[1], state.scissorBox[2], state.scissorBox[3]);
		if (state.scissorTestEnabled) {
			glEnable(GL_SCISSOR_TEST);
		}
		else {
			glDisable(GL_SCISSOR_TEST);
		}

		if (state.cullFaceEnabled) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		glPolygonMode(GL_FRONT_AND_BACK, state.polygonMode[0]);
	}

	static OpenGLState state;
};
OpenGLState OpenGLStateHelper::state = OpenGLState();
