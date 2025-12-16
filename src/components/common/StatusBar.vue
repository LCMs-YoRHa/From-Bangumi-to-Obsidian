<!-- src/components/common/StatusBar.vue -->
<!-- 底部状态栏 -->
<script setup lang="ts">
import { computed } from 'vue';
import { useTaskStore } from '@/stores/task';

const taskStore = useTaskStore();

const statusText = computed(() => {
  switch (taskStore.status) {
    case 'running':
      return '正在执行...';
    case 'completed':
      return '任务完成';
    case 'error':
      return '任务出错';
    default:
      return '就绪';
  }
});

const statusClass = computed(() => ({
  'status-running': taskStore.isRunning,
  'status-completed': taskStore.isCompleted,
  'status-error': taskStore.isError,
}));
</script>

<template>
  <footer class="status-bar">
    <div class="status-info">
      <span class="status-indicator" :class="statusClass"></span>
      <span class="status-text">{{ statusText }}</span>
    </div>
    <div class="status-progress" v-if="taskStore.isRunning && taskStore.progress">
      <span class="progress-text">
        {{ taskStore.progress.current }} / {{ taskStore.progress.total }}
      </span>
      <span class="progress-percentage">
        {{ taskStore.progressPercentage }}%
      </span>
    </div>
  </footer>
</template>

<style scoped>
.status-bar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 8px 16px;
  background: #f5f5f5;
  border-top: 1px solid #e0e0e0;
  font-size: 0.875rem;
}

.status-info {
  display: flex;
  align-items: center;
  gap: 8px;
}

.status-indicator {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: #9e9e9e;
}

.status-indicator.status-running {
  background: #2196f3;
  animation: pulse 1s infinite;
}

.status-indicator.status-completed {
  background: #4caf50;
}

.status-indicator.status-error {
  background: #f44336;
}

@keyframes pulse {
  0%, 100% {
    opacity: 1;
  }
  50% {
    opacity: 0.5;
  }
}

.status-text {
  color: #666;
}

.status-progress {
  display: flex;
  gap: 16px;
  color: #666;
}

.progress-percentage {
  font-weight: 500;
  color: #2196f3;
}
</style>
