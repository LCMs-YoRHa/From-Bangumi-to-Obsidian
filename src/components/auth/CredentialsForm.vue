<!-- src/components/auth/CredentialsForm.vue -->
<!-- 认证信息表单 -->
<script setup lang="ts">
import { ref, computed } from 'vue';
import { useAuthStore } from '@/stores/auth';
import { IconKey, IconFolder, IconSave } from '@/components/icons';

const authStore = useAuthStore();

const localUserId = ref(authStore.userId);
const localToken = ref(authStore.token);
const showToken = ref(false);

const isValid = computed(() => {
  return localUserId.value.trim() !== '' && localToken.value.trim() !== '';
});

const statusMessage = computed(() => {
  if (authStore.isValidating) return '验证中...';
  if (authStore.validationError) return authStore.validationError;
  if (authStore.isValidated) return '认证成功';
  return '';
});

const statusClass = computed(() => ({
  'status-validating': authStore.isValidating,
  'status-success': authStore.isValidated,
  'status-error': !!authStore.validationError,
}));

async function handleSave() {
  authStore.setCredentials(localUserId.value, localToken.value);
  authStore.saveToStorage();
}

async function handleValidate() {
  authStore.setCredentials(localUserId.value, localToken.value);
  await authStore.validateCredentials();
  if (authStore.isValidated) {
    authStore.saveToStorage();
  }
}

async function handleLoadFromFile() {
  // TODO: 实现文件选择对话框
  alert('文件选择功能待实现');
}

async function handleSaveToFile() {
  // TODO: 实现文件保存对话框
  alert('文件保存功能待实现');
}

function handleClear() {
  localUserId.value = '';
  localToken.value = '';
  authStore.clearCredentials();
  authStore.clearStorage();
}

// 初始化时从 localStorage 恢复
authStore.restoreFromStorage();
localUserId.value = authStore.userId;
localToken.value = authStore.token;
</script>

<template>
  <div class="credentials-form">
    <h3 class="form-title"><IconKey :size="18" class="title-icon" /> 认证信息</h3>
    
    <div class="form-group">
      <label for="userId" class="form-label">用户 ID</label>
      <input
        id="userId"
        v-model="localUserId"
        type="text"
        class="form-input"
        placeholder="请输入 Bangumi 用户 ID"
      />
    </div>

    <div class="form-group">
      <label for="token" class="form-label">API Token</label>
      <div class="token-input-wrapper">
        <input
          id="token"
          v-model="localToken"
          :type="showToken ? 'text' : 'password'"
          class="form-input"
          placeholder="请输入 Bangumi API Token"
        />
        <button
          type="button"
          class="toggle-visibility"
          @click="showToken = !showToken"
        >
          <svg v-if="showToken" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M17.94 17.94A10.07 10.07 0 0 1 12 20c-7 0-11-8-11-8a18.45 18.45 0 0 1 5.06-5.94M9.9 4.24A9.12 9.12 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19m-6.72-1.07a3 3 0 1 1-4.24-4.24"/>
            <line x1="1" y1="1" x2="23" y2="23"/>
          </svg>
          <svg v-else width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/>
            <circle cx="12" cy="12" r="3"/>
          </svg>
        </button>
      </div>
    </div>

    <div class="form-status" v-if="statusMessage" :class="statusClass">
      {{ statusMessage }}
    </div>

    <div class="form-actions">
      <button
        class="btn btn-primary"
        :disabled="!isValid || authStore.isValidating"
        @click="handleValidate"
      >
        {{ authStore.isValidating ? '验证中...' : '验证并保存' }}
      </button>
      <button class="btn btn-secondary" @click="handleSave" :disabled="!isValid">
        保存
      </button>
      <button class="btn btn-outline" @click="handleLoadFromFile">
        <IconFolder :size="14" class="btn-icon" /> 从文件加载
      </button>
      <button class="btn btn-outline" @click="handleSaveToFile" :disabled="!isValid">
        <IconSave :size="14" class="btn-icon" /> 保存到文件
      </button>
      <button class="btn btn-danger" @click="handleClear">
        清除
      </button>
    </div>
  </div>
</template>

<style scoped>
.credentials-form {
  background: white;
  border-radius: 8px;
  padding: 20px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.form-title {
  margin: 0 0 16px;
  font-size: 1.125rem;
  color: #333;
  display: flex;
  align-items: center;
  gap: 8px;
}

.title-icon {
  color: #667eea;
}

.btn-icon {
  vertical-align: middle;
}

.form-group {
  margin-bottom: 16px;
}

.form-label {
  display: block;
  margin-bottom: 6px;
  font-size: 0.875rem;
  font-weight: 500;
  color: #555;
}

.form-input {
  width: 100%;
  padding: 10px 12px;
  border: 1px solid #ddd;
  border-radius: 6px;
  font-size: 0.9rem;
  transition: border-color 0.2s, box-shadow 0.2s;
}

.form-input:focus {
  outline: none;
  border-color: #667eea;
  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
}

.token-input-wrapper {
  position: relative;
  display: flex;
}

.token-input-wrapper .form-input {
  padding-right: 40px;
}

.toggle-visibility {
  position: absolute;
  right: 8px;
  top: 50%;
  transform: translateY(-50%);
  background: none;
  border: none;
  cursor: pointer;
  font-size: 1rem;
  padding: 4px;
}

.form-status {
  padding: 10px;
  border-radius: 6px;
  font-size: 0.875rem;
  margin-bottom: 16px;
}

.form-status.status-validating {
  background: #e3f2fd;
  color: #1976d2;
}

.form-status.status-success {
  background: #e8f5e9;
  color: #388e3c;
}

.form-status.status-error {
  background: #ffebee;
  color: #d32f2f;
}

.form-actions {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.btn {
  padding: 8px 16px;
  border-radius: 6px;
  font-size: 0.875rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s;
  border: none;
}

.btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.btn-primary {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
}

.btn-primary:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(102, 126, 234, 0.4);
}

.btn-secondary {
  background: #f5f5f5;
  color: #333;
}

.btn-secondary:hover:not(:disabled) {
  background: #e0e0e0;
}

.btn-outline {
  background: white;
  border: 1px solid #ddd;
  color: #666;
}

.btn-outline:hover:not(:disabled) {
  background: #f5f5f5;
  border-color: #ccc;
}

.btn-danger {
  background: #f44336;
  color: white;
}

.btn-danger:hover:not(:disabled) {
  background: #d32f2f;
}
</style>
