package overlay;

import graphics.*;
import input.Input;
import input.KeyboardInput;
import math.Mat4;
import math.Vec2;
import math.Vec3;
import math.Vec4;
import statemachine.StateMachine;

import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;

public class Overlay extends Drawable {
    public Overlay(int width, int height){
        this.Enabled = false;
        m_position = new Vec3(0, 0, 0);
        m_width = width;
        m_height = height;

        m_shader = new Shader("res/shaders/base.vert", "res/shaders/base.frag");
        m_shader.Enable();
        m_shader.SetUniformMat4f("pr_matrix", Mat4.Orthographic(0, Frame.GetWidth(), 0, Frame.GetHeight(), -1.0f, 1.0f));
        m_shader.Disable();


        float[] vertices = {
                m_position.x,           m_position.y,            1.0f,
                m_position.x,           m_position.y + m_height, 1.0f,
                m_position.x + m_width, m_position.y + m_height, 1.0f,
                m_position.x + m_width, m_position.y,            1.0f
        };



        m_color = new Vec4(0.9f, .9f, 0.9f, 0.3f);

        byte[] indices = {
                0, 1, 2,
                2, 3, 0
        };

        m_vao = new VertexArray();

        m_vao.AddBuffer(new VertexBuffer(vertices, 3), 0);
        m_vao.AddBuffer(new VertexBuffer(m_color, 4), 1);

        m_ibo = new IndexBuffer(indices);

    }

    @Override
    public void Draw(){
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        m_shader.Enable();
        m_vao.Bind();
        m_ibo.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        m_ibo.Unbind();
        m_vao.Unbind();
        m_shader.Disable();
        glDisable(GL_BLEND);
    }

    public void OverlayUpdate(){
        if (Input.IsKeyPressed(GLFW_KEY_TAB)) {
            this.Enabled = !this.Enabled;
        }
    }

    private VertexArray m_vao;
    private IndexBuffer m_ibo;

    private int m_width, m_height;
    public boolean Enabled;
}
